#pragma once

#include "nacl/gl.h"
#include "context.h"

namespace aroma {
	class Renderer;

	class OpenGLContext : public pp::Graphics3DClient, public GLContext {
		protected:
			bool flushing;

			Renderer *renderer;
			const struct PPB_OpenGLES2* gles2_interface;
			pp::Instance *instance;
			pp::Graphics3D graphics;
			pp::Size size;

		public:
			OpenGLContext(pp::Instance* instance);
			virtual ~OpenGLContext();
			void Graphics3DContextLost();

			void set_renderer(Renderer *renderer);

			bool make_current();
			// void resize(const pp::Size& s);
			void resize(const int w, const int h);
			void flush();
			void render(); // tell the renderer to tick

			bool is_flushing();

			int width();
			int height();
			double get_time();
	};
}



