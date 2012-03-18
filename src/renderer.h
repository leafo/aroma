
#include "context.h"
#include "lua_binding.h"
#include "geometry.h"
#include "shader.h"

namespace aroma {
	class Renderer : public Bindable {
		protected:
			GLContext* context;
			LuaBinding* binding;
			Shader* default_shader;
			double last_time;

		public:
			Color current_color;

			Renderer(GLContext* context, LuaBinding* binding);

			bool init();
			void draw(double dt);
			void tick();

			void reshape(const int w, const int h);

			void rect(float x1, float y1, float x2, float y2);

			void bind_all(lua_State *l);
			const char* module_name();

			static int _setColor(lua_State *l);
			static int _rectangle(lua_State *l);
	};
}
