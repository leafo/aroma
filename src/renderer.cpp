
#include "renderer.h"
#include "lib/renderer_support.lua.h"

namespace aroma {
	GLuint init_float_buffer(size_t size, GLenum type=GL_DYNAMIC_DRAW) {
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, NULL, type);
		return buffer;
	}

	template <typename T>
	void update_float_buffer(GLuint buffer, T* data, size_t count=1) {
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T) * count, data);
	}

	void Renderer::img_rect(const Image* img, const Transform& t) {
		Quad q = {0,0, 1,1};
		img_rect_blit(img, q, t);
	}

	void Renderer::img_rect_blit(const Image* img, const Quad& q,
			const Transform& t)
	{
		TexQuadCoords quad = TexQuadCoords::from_rect(
			 t.x, t.y, img->width * q.width(), img->height * q.height(),
			 q.x1, q.y1, q.width(), q.height()
		);

		bool pop_mat = false;
		if (t.needs_mat()) {
			t.push(projection);
			pop_mat = true;
		}

		default_shader->set_uniform("C", current_color);
		projection.apply(default_shader);

		update_float_buffer(tex_quad_buffer, &quad);

		default_shader->set_uniform("texturing", 1u);
		img->bind();
		default_shader->set_uniform("tex", 0u);

		GLuint P = default_shader->attr_loc("P");
		GLuint T = default_shader->attr_loc("T");

		size_t stride = sizeof(float) * 4;

		glEnableVertexAttribArray(P);
		glVertexAttribPointer(P, 2, GL_FLOAT, false, stride, 0);

		glEnableVertexAttribArray(T);
		glVertexAttribPointer(T, 2, GL_FLOAT, false, stride,
				(void*)(sizeof(float) * 2));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (pop_mat) projection.pop();
	}

	// TODO: convert to tile map
	void Renderer::font_write(const Font* font, int x, int y,
		const char *str)
	{
		size_t len = strlen(str);

		TexQuadCoords quads[len];

		int per_row = font->letter_tex.width / font->max_width;

		for (int i = 0; i < len; ++i) {
			int tid = font->letter_map[str[i] - font->start_i];
			const Letter l = font->letters[tid];

			int tx = (tid % per_row) * font->max_width;
			int ty = (tid / per_row) * font->line_height;

			quads[i] = TexQuadCoords::from_rect(
				x, y, l.width, font->line_height,

				font->letter_tex,
				tx, ty, l.width, font->line_height
			);

			x += l.width;
		}

		default_shader->set_uniform("C", current_color);
		projection.apply(default_shader);

		glBindBuffer(GL_ARRAY_BUFFER, coord_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TexQuadCoords) * len,
				&quads, GL_STREAM_DRAW);

		// ready 2 draw

		default_shader->set_uniform("texturing", 1u);
		font->letter_tex.bind();
		default_shader->set_uniform("tex", 0u);

		GLuint P = default_shader->attr_loc("P");
		GLuint T = default_shader->attr_loc("T");

		size_t stride = sizeof(float) * 4;

		glEnableVertexAttribArray(P); // bind vert
		glVertexAttribPointer(P, 2, GL_FLOAT, false, stride, 0);

		glEnableVertexAttribArray(T); // bind tex
		glVertexAttribPointer(T, 2, GL_FLOAT, false, stride,
				(void*)(sizeof(float) * 2));

		for (int i = 0; i < len; i++) {
			glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Renderer::rect(float x, float y, float w, float h) {
		QuadCoords quad = QuadCoords::from_rect(x, y, w, h);
		update_float_buffer(quad_buffer, &quad);
		draw_primitive(GL_TRIANGLE_STRIP, 4);
	}

	void Renderer::rect_line(float x1, float y1, float w, float h) {
		float x2 = x1 + w, y2 = y1 + h;

		float verts[] = {
			x1, y1,
			x2, y1,
			x2, y2,
			x1, y2,
		};

		update_float_buffer(quad_buffer, verts, 8);
		draw_primitive(GL_LINE_LOOP, 4);
	}

	void Renderer::draw_primitive(GLenum type, size_t vertices) {
		default_shader->set_uniform("C", current_color);
		projection.apply(default_shader);

		default_shader->set_uniform("texturing", 0u);

		GLuint P = default_shader->attr_loc("P");

		glDisableVertexAttribArray(default_shader->attr_loc("T"));

		glEnableVertexAttribArray(P);
		glVertexAttribPointer(P, 2, GL_FLOAT, false, 0, 0);

		glDrawArrays(type, 0, vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Renderer::Renderer(GLContext* context, LuaBinding* binding) :
		context(context),
		binding(binding),
		default_shader(NULL),
		current_font(NULL),
		_texturing(false),
		fps(0), frames(0), elapsed_time(0)
	{
		context->set_renderer(this);
		binding->bind_module(this);
	}

	GLContext* Renderer::get_context() {
		return context;
	}

	int Renderer::get_fps() {
		return fps;
	}

	bool Renderer::init() {
		log("init renderer\n");
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (!binding->load_and_run(
					renderer_support_lua,
					renderer_support_lua_len,
					"renderer_support.lua"))
		{
			return false;
		}

		if (!default_shader) {
			err("you forgot to create a default shader!\n");
			return false;
		}

		quad_buffer = init_float_buffer(8);
		tex_quad_buffer = init_float_buffer(16);

		glGenBuffers(1, &coord_buffer);

		last_time = context->get_time();
		return true;
	}

	void Renderer::draw(double dt) {
		glClear(GL_COLOR_BUFFER_BIT);

		if (!default_shader) return;

		// load uniforms
		default_shader->bind();

		lua_State* l = binding->lua();

		int top = lua_gettop(l);

		lua_pushnumber(l, dt);
		binding->send_event("update", 1);
		binding->send_event("draw", 0);

		lua_settop(l, top);
	}

	// called for every frame
	void Renderer::tick() {
		context->make_current();

		glViewport(0, 0, context->width(), context->height());
		projection.reset(Mat4::ortho2d(0, context->width(), 0, context->height()));

		double time = context->get_time();
		double dt = time - last_time;

		++frames;
		elapsed_time += dt;
		if (elapsed_time > FPS_UPDATE_FREQ) {
			fps = frames / elapsed_time + 0.5;
			elapsed_time -= FPS_UPDATE_FREQ;
			frames = 0;
		}

		draw(dt);
		last_time = time;

		context->flush();
	}

	void Renderer::reshape(const int w, const int h) {
		context->resize(w, h);
		if (!context->is_flushing()) {
			tick(); // start it up
		}
	}

	const char* Renderer::module_name() {
		return "graphics";
	}

	void Renderer::texturing(bool enabled) {
		_texturing = enabled;
	}

	// write all the funcs into the current table
	void Renderer::bind_all(lua_State *l) {
		set_new_func("setColor", _setColor);
		set_new_func("getColor", _getColor);
		set_new_func("setBackgroundColor", _setBackgroundColor);

		set_new_func("getWidth", _getWidth);
		set_new_func("getHeight", _getHeight);

		set_new_func("reset", _reset);

		set_new_func("rectangle", _rectangle);
		set_new_func("draw", _draw);
		set_new_func("drawq", _drawq);
		set_new_func("setDefaultShader", _setDefaultShader);

		set_new_func("getFont", _getFont);
		set_new_func("setFont", _setFont);

		set_new_func("setLineWidth", _setLineWidth);

		set_new_func("push", _push);
		set_new_func("pop", _pop);
		set_new_func("translate", _translate);
		set_new_func("scale", _scale);
		set_new_func("rotate", _rotate);

		set_new_func("print", _print);

		set_new_func("newQuad", Quad::_new);
		set_new_func("newShader", Shader::_new);
		set_new_func("newImage", Image::_new);
	}

	int Renderer::_setColor(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		self->current_color = Color::read(l, 1);
		return 0;
	}

	int Renderer::_getColor(lua_State* l) {
		return upvalue_self(Renderer)->current_color.push(l);
	}

	int Renderer::_setBackgroundColor(lua_State* l) {
		Color c = Color::read(l, 1);
		glClearColor(c.rf(), c.gf(), c.bf(), 1.0);
		return 0;
	}

	int Renderer::_getWidth(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		lua_pushnumber(l, self->get_context()->width());
		return 1;
	}

	int Renderer::_getHeight(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		lua_pushnumber(l, self->get_context()->height());
		return 1;
	}

	int Renderer::_rectangle(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);

		if (lua_type(l, 1) == LUA_TSTRING) {
			Rect r = Rect::read(l, 2);
			if (strcmp(lua_tostring(l, 1), "line") == 0) {
				self->rect_line(r.x, r.y, r.w, r.h);
			} else { // fill
				self->rect(r.x, r.y, r.w, r.h);
			}
			return 0;
		}

		Rect r = Rect::read(l, 1);
		self->rect(r.x, r.y, r.w, r.h);
		return 0;
	}

	// thing, x, y, r, sx, sy, ox, oy
	int Renderer::_draw(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		Transform t = Transform::read(l, 2);

		if (self->binding->is_type(l, 1, "Image")) {
			self->img_rect(getself(Image), t);
		} else {
			return luaL_error(l, "unknown value passed to draw");
		}
		return 0;
	}

	int Renderer::_drawq(lua_State *l) {
		Renderer* self = upvalue_self(Renderer);
		Quad* q = getselfi(Quad, 2);
		Transform t = Transform::read(l, 3);

		if (self->binding->is_type(l, 1, "Image")) {
			self->img_rect_blit(getself(Image), *q, t);
		} else {
			return luaL_error(l, "unknown value passed to drawq");
		}
		return 0;
	}

	int Renderer::_setDefaultShader(lua_State *l) {
		Renderer* self = upvalue_self(Renderer);
		Shader* shader = getself(Shader);
		if (shader->link()) {
			self->default_shader = shader;
		}

		self->binding->store_in_registry(l, 1, "default_shader");
		return 0;
	}

	int Renderer::_setFont(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		Font* font = getself(Font);
		self->current_font = font;
		self->binding->store_in_registry(l, 1, "current_font");
		return 0;
	}

	int Renderer::_getFont(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		return self->binding->from_registry(l, "current_font");
	}

	int Renderer::_setLineWidth(lua_State* l) {
		double width = luaL_checknumber(l, 1);
		glLineWidth(width);
		return 0;
	}

	int Renderer::_push(lua_State* l) {
		upvalue_self(Renderer)->projection.push();
		return 0;
	}

	int Renderer::_pop(lua_State* l) {
		upvalue_self(Renderer)->projection.pop();
		return 0;
	}

	int Renderer::_translate(lua_State* l) {
		Point p = Point::pop(l);
		upvalue_self(Renderer)->projection.mul(Mat4::translate(p.x, p.y));
		return 0;
	}

	int Renderer::_scale(lua_State* l) {
		Point p = Point::pop(l);
		upvalue_self(Renderer)->projection.mul(Mat4::scale(p.x, p.y));
		return 0;
	}

	int Renderer::_rotate(lua_State* l) {
		double theta = luaL_checknumber(l, 1);
		upvalue_self(Renderer)->projection.mul(Mat4::rotate2d(theta));
		return 0;
	}

	int Renderer::_reset(lua_State *l) {
		Renderer* r = upvalue_self(Renderer);
		r->current_color = Color(255,255,255);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.1, 0.1, 0.1, 1.0);
		return 0;
	}

	int Renderer::_print(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		Font* font = self->current_font;

		int start = 1;
		if (self->binding->is_type(l, 1, "Font")) {
			font = getself(Font);
			start++;
		}

		const char* str = luaL_checkstring(l, start);
		Point p = Point::read2d(l, start+1);

		if (font) {
			self->font_write(font, p.x, p.y, str);
		}
		return 0;
	}

	int Quad::_new(lua_State* l) {
		Quad q;

		double x = luaL_checknumber(l, 1);
		double y = luaL_checknumber(l, 2);
		double w = luaL_checknumber(l, 3);
		double h = luaL_checknumber(l, 4);

		double sw = luaL_checknumber(l, 5);
		double sh = luaL_checknumber(l, 6);

		if (sw <= 0 || sh <= 0) {
			return luaL_error(l, "Reference width and height must be greater than 0");
		}

		q.x1 = x / sw;
		q.y1 = y / sw;

		q.x2 = (x + w) / sw;
		q.y2 = (y + h) / sw;

		return q.push(l);
	}

	int Quad::_flip(lua_State* l) {
		Quad *self = getself(Quad);

		int flip_x = lua_toboolean(l, 2);
		int flip_y = lua_toboolean(l, 3);

		Quad out = *self;
		if (flip_x) {
			double tmp = out.x1;
			out.x1 = out.x2;
			out.x2 = tmp;
		}

		if (flip_y) {
			double tmp = out.y1;
			out.y1 = out.y2;
			out.y2 = tmp;
		}

		return out.push(l);
	}

	int Quad::push(lua_State* l) {
		*newuserdata(Quad) = *this;

		if (luaL_newmetatable(l, "Quad")) {
			lua_newtable(l);
			setfunction("flip", _flip);
			lua_setfield(l, -2, "__index");
		}

		lua_setmetatable(l, -2);
		return 1;
	}

	double Quad::width() const {
		return x2 - x1;
	}

	double Quad::height() const {
		return y2 - y1;
	}

	QuadCoords Quad::quad_coords() const {
		QuadCoords out = {{
			x1, y1,
			x2, y1,
			x1, y2,
			x2, y2
		}};
		return out;
	}

	QuadCoords QuadCoords::from_rect(float x1, float y1, float w, float h) {
		float x2 = x1 + w, y2 = y1 + h;

		QuadCoords out = {{
			x1, y1,
			x2, y1,
			x1, y2,
			x2, y2
		}};
		return out;
	}

	TexQuadCoords TexQuadCoords::from_rect(float x1, float y1, float w, float h,
			float sx, float sy, float sw, float sh)
	{
		float x2 = x1 + w, y2 = y1 + h;
		float sx2 = sx + sw, sy2 = sy + sh;

		TexQuadCoords out = {{
			x1, y1, sx, sy,
			x2, y1, sx2, sy,
			x1, y2, sx, sy2,
			x2, y2, sx2, sy2
		}};

		return out;
	}

	TexQuadCoords TexQuadCoords::from_rect(float x, float y, float w, float h,
				const Image & img, float tx, float ty, float tw, float th)
	{
		return from_rect(x, y, w, h, tx / img.width, ty / img.height,
				tw / img.width, th / img.height);
	}

	// used for draw and drawq
	Transform Transform::read(lua_State* l, int i) {
		int nargs = lua_gettop(l);
		int max_i = i + 6; // reads 7 items

		Transform t = { 0, 0, 0, 1, 1, 0, 0 };

		t.x = lua_tonumber(l, i);
		t.y = lua_tonumber(l, i + 1);

		nargs = nargs > max_i ? max_i : nargs;

		switch (nargs - i) {
			case 6: if (!lua_isnil(l, i + 6)) t.oy = lua_tonumber(l, i + 6);
			case 5: if (!lua_isnil(l, i + 5)) t.ox = lua_tonumber(l, i + 5);
			case 4: if (!lua_isnil(l, i + 4)) t.sy = lua_tonumber(l, i + 4);
			case 3: if (!lua_isnil(l, i + 3)) t.sx = lua_tonumber(l, i + 3);
			case 2: if (!lua_isnil(l, i + 2)) t.r =  lua_tonumber(l, i + 2);
		}

		return t;
	}

	bool Transform::needs_mat() const {
		return r != 0 || ox != 0 || oy != 0 || sx != 1 || sy != 1;
	}

	void Transform::push(MatrixStack& proj) const {
		proj.push(Mat4::translate(x, y)); // back
		if (r != 0) proj.mul(Mat4::rotate2d(r));
		if (sx != 1 || sy != 1) proj.mul(Mat4::scale(sx, sy));
		proj.mul(Mat4::translate(-(x + ox), -(y+ oy))); // to origin
	}

}
