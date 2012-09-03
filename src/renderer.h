#pragma once

#include "context.h"
#include "lua_binding.h"
#include "geometry.h"
#include "shader.h"
#include "image.h"
#include "font.h"

namespace aroma {
	const static double FPS_UPDATE_FREQ = 1.0;

	struct TexQuadCoords {
		GLfloat coords[16];
		static TexQuadCoords from_rect(float x, float y, float w, float h,
				float sx, float sy, float sw, float sh);

		static TexQuadCoords from_rect(float x, float y, float w, float h,
				const Image & img, float tx, float ty, float tw, float th);
	};

	struct QuadCoords {
		GLfloat coords[8];
		static QuadCoords from_rect(float x, float y, float w, float h);
	};

	struct Quad {
		double x1, y1, x2, y2;

		double width() const;
		double height() const;

		QuadCoords quad_coords() const;

		static int _new(lua_State* l);
		static int _flip(lua_State* l);

		int push(lua_State* l);
	};

	// the transformation used by draw and drawq
	struct Transform {
		double x, y, r, sx, sy, ox, oy;

		bool needs_mat() const;
		void push(MatrixStack& proj) const;

		void print() const;

		static Transform read(lua_State* l, int i);
	};

	class Renderer : public Bindable {
		protected:
			GLContext* context;
			LuaBinding* binding;
			Shader* default_shader;
			Font* current_font;

			double last_time;
			bool _texturing;
			MatrixStack projection;

			GLuint quad_buffer;
			GLuint tex_quad_buffer;
			GLuint coord_buffer;

			// for fps calculation
			int fps;
			int frames;
			double elapsed_time;

			// draw untextured vertices that are bound to current buffer
			void draw_primitive(GLenum type=GL_TRIANGLE_STRIP,
				size_t vertices=4);

		public:
			Color current_color;

			Renderer(GLContext* context, LuaBinding* binding);

			bool init();
			void draw(double dt);
			void tick();

			void reshape(const int w, const int h);

			void rect(float x1, float y1, float w, float h);
			void rect_line(float x1, float y1, float w, float h);

			void img_rect(const Image* i, const Transform& t);
			void img_rect_blit(const Image* i, const Quad& q, const Transform& t);
			void font_write(const Font* font, int x, int y, const char *str);

			void texturing(bool enabled);

			GLContext* get_context();
			int get_fps();

			void bind_all(lua_State *l);
			const char* module_name();

			static int _setColor(lua_State* l);
			static int _getColor(lua_State* l);

			static int _setBackgroundColor(lua_State* l);

			static int _getWidth(lua_State* l);
			static int _getHeight(lua_State* l);

			static int _reset(lua_State *l);

			static int _rectangle(lua_State *l);
			static int _draw(lua_State *l);
			static int _drawq(lua_State *l);

			static int _push(lua_State* l);
			static int _pop(lua_State* l);
			static int _translate(lua_State* l);
			static int _scale(lua_State* l);
			static int _rotate(lua_State* l);

			static int _setDefaultShader(lua_State *l);

			static int _setFont(lua_State *l);
			static int _getFont(lua_State *l);

			static int _setLineWidth(lua_State *l);

			static int _print(lua_State* l);
	};
}
