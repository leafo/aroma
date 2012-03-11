
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

using namespace std;

namespace aroma {
  class AromaInstance : public pp::Instance {
    lua_State *l;
    public:
    explicit AromaInstance(PP_Instance instance) : pp::Instance(instance) {
      l = luaL_newstate();
      luaL_openlibs(l);
      luaL_loadstring(l, "return 'hello from lua! '");
      lua_call(l, 0, 1);
      PostMessage(pp::Var(luaL_checkstring(l, -1)));
      lua_settop(l, 0);
    }

    virtual ~AromaInstance() { }

    void HandleMessage(const pp::Var& var_message) { }

    void DidChangeView(const pp::Rect& pos, const pp::Rect* clip) {
      PostMessage(pp::Var("DidChangeView"));
    }
  };

  class AromaModule : public pp::Module {
    public:
      AromaModule() : pp::Module() {}
      virtual ~AromaModule() {}

      virtual pp::Instance* CreateInstance(PP_Instance instance) {
        return new AromaInstance(instance);
      }
  };
}

namespace pp {
  Module* CreateModule() {
    return new aroma::AromaModule();
  }
}
