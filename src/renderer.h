
#include "context.h"
#include "lua_binding.h"
#include "geometry.h"
#include "shader.h"
#include "nacl/image.h"

namespace aroma {
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

			// r: rotation angle in degrees
			void img_rect(const Image* i, float x, float y, float r=0, float sx=1,
					float sy=1, float ox=0, float oy=0);

			void texturing(bool enabled);

			void bind_all(lua_State *l);
			const char* module_name();


			static int _setColor(lua_State *l);
			static int _rectangle(lua_State *l);
			static int _draw(lua_State *l);

			static int _setDefaultShader(lua_State *l);
	};
}
