
#pragma once

#include <cstdio>
#include <string>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "ppapi/cpp/rect.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


#include <sys/time.h>
#include <sys/nacl_syscalls.h>

using namespace std;

extern "C" int luaopen_cjson(lua_State *l);

// opengl headers
#include <GLES2/gl2.h>
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/cpp/graphics_3d_client.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/gles2/gl2ext_ppapi.h"

#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/rect.h"
// ~

namespace aroma {
  void push_var(lua_State* l, pp::Var var);
  pp::Var to_var(lua_State* l, int index);
	
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
			bool make_current();
			void resize(const pp::Size& s);
			void flush();
			void Graphics3DContextLost();
			void render(); // tell the renderer to tick

			int width();
			int height();
	};

	class Renderer {
    protected:
      OpenGLContext* context;
			pp::Instance* instance;

		public:
      Renderer(pp::Instance* instance);
      void draw();
      void tick();
      void did_change_view(const pp::Rect& pos, const pp::Rect& clip);
	};
}


