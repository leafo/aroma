#pragma once

#include "nacl/gl.h"

namespace aroma {
	class Renderer;

	class OpenGLContext : public pp::Graphics3DClient {
		protected:
			Renderer *renderer;
			const struct PPB_OpenGLES2* gles2_interface;
			pp::Instance *instance;
			pp::Graphics3D graphics;
			pp::Size size;

		public:
			OpenGLContext(pp::Instance* instance, Renderer* renderer);
			virtual ~OpenGLContext();
			void Graphics3DContextLost();

			bool make_current();
			void resize(const pp::Size& s);
			void flush();
			void render(); // tell the renderer to tick

			int width();
			int height();
	};
}



