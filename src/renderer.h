
#include "context.h"
#include "shader.h"

namespace aroma {
	class Renderer {
		protected:
			GLContext* context;
			Shader* default_shader;

		public:
			Renderer(GLContext* context);

			void init();
			void draw();
			void tick();

			void reshape(const int w, const int h);

			void rect(float x1, float y1, float x2, float y2);
	};
}
