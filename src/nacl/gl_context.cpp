
#include "nacl/aroma.h"
#include "nacl/gl_context.h"
#include "renderer.h"
#include "common.h"

namespace aroma {

	void default_flush_callback(void *data, int32_t result) {
		OpenGLContext* context = (OpenGLContext*)data;
		context->render();
	}

	OpenGLContext::OpenGLContext(pp::Instance* instance) :
		flushing(false),
		pp::Graphics3DClient(instance),
		instance(instance)
	{
		pp::Module *module = pp::Module::Get();
		assert(module);
		gles2_interface = static_cast<const struct PPB_OpenGLES2*>(
				module->GetBrowserInterface(PPB_OPENGLES2_INTERFACE));
		assert(gles2_interface);
	}

	OpenGLContext::~OpenGLContext() {
		glSetCurrentContextPPAPI(0);
	}

	bool OpenGLContext::make_current() {
		bool is_init = graphics.is_null();
		if (graphics.is_null()) {
			log("init graphics\n");
			int32_t attribs[] = {
				PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
				PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
				PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
				PP_GRAPHICS3DATTRIB_SAMPLES, 0,
				PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
				PP_GRAPHICS3DATTRIB_WIDTH, size.width(),
				PP_GRAPHICS3DATTRIB_HEIGHT, size.height(),
				PP_GRAPHICS3DATTRIB_NONE
			};
			graphics = pp::Graphics3D(instance, pp::Graphics3D(), attribs);
			if (graphics.is_null()) {
				glSetCurrentContextPPAPI(0);
				return false;
			}
			instance->BindGraphics(graphics);
		}

		glSetCurrentContextPPAPI(graphics.pp_resource());

		if (is_init && !renderer->init()) {
			return false;
		}

		return true;
	}

	void OpenGLContext::resize(const int width, const int height) {
		log("resize buffers\n");
		size = pp::Size(width, height);
		if (!graphics.is_null()) {
			graphics.ResizeBuffers(width, height);
		}
	}

	void OpenGLContext::flush() {
		flushing = true;
		graphics.SwapBuffers(pp::CompletionCallback(&default_flush_callback, this));
	}

	void OpenGLContext::Graphics3DContextLost() {
		assert(!"++ Lost graphics context");
	}

	void OpenGLContext::render() {
		flushing = false;
		renderer->tick();
	}

	int OpenGLContext::width() {
		return size.width();
	}

	int OpenGLContext::height() {
		return size.height();
	}

	double OpenGLContext::get_time() {
		return pp::Module::Get()->core()->GetTimeTicks();
	}

	void OpenGLContext::set_renderer(Renderer *r) {
		renderer = r;
	}

	bool OpenGLContext::is_flushing() {
		return flushing;
	}
}
