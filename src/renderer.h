
#include "context.h"
#include "geometry.h"
#include "shader.h"

namespace aroma {
	class Renderer {
		protected:
			GLContext* context;
			Shader* default_shader;

		public:
			Color current_color;

			Renderer(GLContext* context);

			bool init();
			void draw();
			void tick();

			void reshape(const int w, const int h);

			void rect(float x1, float y1, float x2, float y2);

			void bind_all(lua_State *l);
			static int _setColor(lua_State *l);
			static int _rectangle(lua_State *l);
	};
}
