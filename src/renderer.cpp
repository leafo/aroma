
#include "renderer.h"

namespace aroma {

	GLuint make_float_buffer(float* parts, size_t size) {
		GLuint buffer;
		glGenBuffers(1, &buffer);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, parts, GL_STATIC_DRAW);
		return buffer;
	}

	void Renderer::rect(float x1, float y1, float x2, float y2) {
		// float colors[] = {
		// 	1,1,1,
		// 	1,1,1,
		// 	1,1,1,
		// 	1,1,1,
		// };

		float tex_coords[] = {
			0,0,
			1,0,
			0,1,
			1,1
		};

		float verts[] = {
			x1,y1,
			x2,y1,
			x1,y2,
			x2,y2
		};

		// GLuint buffs[2];
		GLuint vert_buffer = make_float_buffer(verts, 8);
		GLuint tex_buffer = make_float_buffer(tex_coords, 8);

		GLuint P = default_shader->attr_loc("P");
		GLuint T = default_shader->attr_loc("T");

		GLuint C = default_shader->uniform_loc("C");
		GLuint tex = default_shader->uniform_loc("tex");

		glUniform4f(C, current_color.rf(), current_color.gf(), current_color.bf(),
				current_color.af());

		glUniform1i(tex, 0); // default texture

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
	}

	Renderer::Renderer(GLContext* context, LuaBinding* binding) :
		context(context),
		binding(binding),
		default_shader(NULL)
	{
		context->set_renderer(this);
		binding->bind_module(this);
	}

	bool Renderer::init() {
		log("init renderer\n");
		glClearColor(0.1, 0.1, 0.1, 1.0);

		const char *vertex_src =
			"uniform vec4 C;\n" // color
			"attribute vec2 P;\n" // vertex
			"attribute vec2 T;\n" // texture coords
			"\n"
			"varying lowp vec4 vColor;\n"
			"varying vec2 vTex;\n"
			"void main(void) {\n"
			"  vTex = T;\n"
			"  vColor = C;\n"
			"  gl_Position = vec4(P, 0.0, 1.0);\n"
			"}\n"
			;

		const char *fragment_src =
			"uniform sampler2D tex;\n"
			"varying lowp vec4 vColor;\n"
			"varying mediump vec2 vTex;\n"
			"\n"
			"void main(void) {\n"
			// "  gl_FragColor = vColor;\n"
			"  gl_FragColor = texture2D(tex, vTex);\n"
			"}\n"
			;

		default_shader = new Shader();
		default_shader->add(GL_VERTEX_SHADER, vertex_src);
		default_shader->add(GL_FRAGMENT_SHADER, fragment_src);
		default_shader->link();

		last_time = context->get_time();
		return true;
	}

	void Renderer::draw(double dt) {
		glClear(GL_COLOR_BUFFER_BIT);
		default_shader->bind();

		lua_State* l = binding->lua();

		int i = lua_gettop(l);
		binding->push_self();

		lua_getfield(l, -1, "update");
		if (!lua_isnil(l, -1)) {
			lua_pushnumber(l, dt);
			lua_call(l, 1, 0);
		} else {
			lua_pop(l, 1); // pop nil
		}

		lua_getfield(l, -1, "draw");
		if (!lua_isnil(l, -1)) {
			lua_call(l, 0, 0);
		} else {
			lua_pop(l, 1); // pop nil
		}

		lua_settop(l, i);
	}

	// called for every frame
	void Renderer::tick() {
		context->make_current();

		glViewport(0, 0, context->width(), context->height());

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

	// write all the funcs into the current table
	void Renderer::bind_all(lua_State *l) {
		set_new_func("setColor", _setColor);
		set_new_func("rectangle", _rectangle);
	}

	int Renderer::_setColor(lua_State *l) {
		Renderer *self = upvalue_self(Renderer);
		self->current_color = Color::pop(l);
		return 0;
	}

	int Renderer::_rectangle(lua_State *l) {
		Renderer *self = upvalue_self(Renderer);
		Rect r = Rect::pop(l);
		self->rect(r.x, r.y, r.x + r.w, r.y + r.h);
		return 0;
	}
}
