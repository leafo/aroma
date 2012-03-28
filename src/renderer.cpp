
#include "renderer.h"
#include "lib/renderer_support.lua.h"

namespace aroma {

	GLuint make_float_buffer(float* parts, size_t size) {
		GLuint buffer;
		glGenBuffers(1, &buffer);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, parts, GL_STATIC_DRAW);
		return buffer;
	}

	void Renderer::img_rect(const Image* img, const Transform& t) {
		Quad q = {0,0, 1,1};
		img_rect_blit(img, q, t);
	}

	void Renderer::img_rect_blit(const Image* img, const Quad& q, const Transform& t) {
		QuadCoords tex_coords = q.quad_coords();

		float x2 = t.x + img->width * q.width();
		float y2 = t.y + img->height * q.height();

		float verts[] = {
			t.x, t.y,
			x2,  t.y,
			t.x, y2,
			x2,  y2
		};

		bool pop_mat = false;
		if (t.needs_mat()) {
			t.push(projection);
			pop_mat = true;
		}

		default_shader->set_uniform("C", current_color);
		projection.apply(default_shader);

		GLuint vert_buffer = make_float_buffer(verts, 8);
		GLuint tex_buffer = make_float_buffer((float*)&tex_coords.coords, 8);

		default_shader->set_uniform("texturing", 1u);
		img->bind();
		default_shader->set_uniform("tex", 0u);

		GLuint P = default_shader->attr_loc("P");
		GLuint T = default_shader->attr_loc("T");

		glEnableVertexAttribArray(P);
		glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
		glVertexAttribPointer(P, 2, GL_FLOAT, false, 0, 0);

		glEnableVertexAttribArray(T);
		glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
		glVertexAttribPointer(T, 2, GL_FLOAT, false, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vert_buffer);
		glDeleteBuffers(1, &tex_buffer);
		if (pop_mat) projection.pop();
	}

	void Renderer::rect(float x1, float y1, float x2, float y2) {
		float verts[] = {
			x1, y1,
			x2, y1,
			x1, y2,
			x2, y2
		};

		default_shader->set_uniform("C", current_color);
		projection.apply(default_shader);

		GLuint vert_buffer = make_float_buffer(verts, 8);

		default_shader->set_uniform("texturing", 0u);

		GLuint P = default_shader->attr_loc("P");

		glDisableVertexAttribArray(default_shader->attr_loc("T"));

		glEnableVertexAttribArray(P);
		glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
		glVertexAttribPointer(P, 2, GL_FLOAT, false, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vert_buffer);
	}

	Renderer::Renderer(GLContext* context, LuaBinding* binding) :
		context(context),
		binding(binding),
		default_shader(NULL),
		_texturing(false)
	{
		context->set_renderer(this);
		binding->bind_module(this);
	}

	bool Renderer::init() {
		log("init renderer\n");
		glClearColor(0.1, 0.1, 0.1, 1.0);

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
		draw(time - last_time);
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
		set_new_func("rectangle", _rectangle);
		set_new_func("draw", _draw);
		set_new_func("drawq", _drawq);
		set_new_func("setDefaultShader", _setDefaultShader);

		set_new_func("push", _push);
		set_new_func("pop", _pop);
		set_new_func("translate", _translate);
		set_new_func("scale", _scale);
		set_new_func("rotate", _rotate);

		set_new_func("newQuad", Quad::_new);

		set_new_func("newShader", Shader::_new);
	}

	int Renderer::_setColor(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		self->current_color = Color::pop(l);
		return 0;
	}

	int Renderer::_getColor(lua_State* l) {
		return upvalue_self(Renderer)->current_color.push(l);
	}

	int Renderer::_rectangle(lua_State* l) {
		Renderer* self = upvalue_self(Renderer);
		Rect r = Rect::pop(l);
		self->rect(r.x, r.y, r.x + r.w, r.y + r.h);
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

		// store the shader in the aroma entry in the registry
		// so it's not garbage collected
		luaL_newmetatable(l, "aroma");
		lua_pushvalue(l, 1);
		lua_setfield(l, -2, "default_shader");

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

		*newuserdata(Quad) = q;

		if (luaL_newmetatable(l, "Quad")) {
			lua_newtable(l);
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
