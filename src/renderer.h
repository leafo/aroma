
#include "context.h"
#include "lua_binding.h"
#include "geometry.h"
#include "shader.h"
#include "nacl/image.h"

namespace aroma {
	struct QuadCoords {
		GLfloat coords[8];
	};

	struct Quad {
		double x1, y1, x2, y2;

		double width() const;
		double height() const;

		QuadCoords quad_coords() const;
		static int _new(lua_State* l);
	};

	// the transformation used by draw and drawq
	struct Transform {
		double x, y, r, sx, sy, ox, oy;

		bool needs_mat() const;
		void push(MatrixStack& proj) const;

		static Transform read(lua_State* l, int i);
	};

	class Renderer : public Bindable {
		protected:
			GLContext* context;
			LuaBinding* binding;
			Shader* default_shader;
			double last_time;

			bool _texturing;
			MatrixStack projection;

		public:
			Color current_color;

			Renderer(GLContext* context, LuaBinding* binding);

			bool init();
			void draw(double dt);
			void tick();

			void reshape(const int w, const int h);

			void rect(float x1, float y1, float x2, float y2);

			void img_rect(const Image* i, const Transform& t);
			void img_rect_blit(const Image* i, const Quad& q, const Transform& t);

			void texturing(bool enabled);

			void bind_all(lua_State *l);
			const char* module_name();


			static int _setColor(lua_State *l);
			static int _getColor(lua_State *l);

			static int _rectangle(lua_State *l);
			static int _draw(lua_State *l);
			static int _drawq(lua_State *l);

			static int _push(lua_State* l);
			static int _pop(lua_State* l);
			static int _translate(lua_State* l);
			static int _scale(lua_State* l);
			static int _rotate(lua_State* l);

			static int _setDefaultShader(lua_State *l);
	};
}
