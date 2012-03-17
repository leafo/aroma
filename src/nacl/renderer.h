
#include "nacl/aroma.h"
#include "shader.h"

namespace aroma {
	class Renderer {
		protected:
			OpenGLContext* context;
			pp::Instance* instance;
			Shader* default_shader;

		public:
			Renderer(pp::Instance* instance);

			void init();
			void draw();
			void tick();
			void did_change_view(const pp::Rect& pos, const pp::Rect& clip);

			void rect(float x1, float y1, float x2, float y2);
	};
}
