
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

#include "nacl.lua.h"

using namespace std;

namespace aroma {

  void push_var(lua_State* l, pp::Var var) {
    if (var.is_null()) {
      lua_pushnil(l);
    } else if (var.is_number()) {
      lua_pushnumber(l, var.AsDouble());
    } else if (var.is_bool()) {
      lua_pushboolean(l, var.AsBool());
    } else {
      lua_pushstring(l, var.AsString().c_str());
    }
  }

  pp::Var to_var(lua_State* l, int index) {
    if (lua_isnil(l, index)) {
      return pp::Var(pp::Var::Null());
    } else if (lua_isnumber(l, index)) {
      return pp::Var(lua_tonumber(l, index));
    } else if (lua_isboolean(l, index)) {
      return pp::Var((bool)lua_toboolean(l, index));
    }

    return pp::Var(lua_tostring(l, index));
  }

  int _post_message(lua_State *l) {
    pp::Instance *instance  = (pp::Instance*)lua_touserdata(l, lua_upvalueindex(1));
    instance->PostMessage(to_var(l, 1));
    return 0;
  }

  class AromaInstance : public pp::Instance {
    protected:
      lua_State* l;
    public:
      AromaInstance(PP_Instance instance) : pp::Instance(instance) { }

      bool Init(uint32_t argc, const char** argn, const char** argv) {
        l = luaL_newstate();
        luaL_openlibs(l);

        lua_newtable(l);
        lua_setglobal(l, "nacl");

        // install methods
        lua_getglobal(l, "nacl");
        lua_pushlightuserdata(l, this);
        lua_pushcclosure(l, _post_message, 1);
        lua_setfield(l, -2, "post_message");

        lua_settop(l, 0);

        if (luaL_loadbuffer(l, (const char*)lib_nacl_lua, lib_nacl_lua_len, "nacl.lua") != 0) {
          fprintf(stderr, "%s\n", luaL_checkstring(l, -1));
          return false;
        }

        if (lua_pcall(l, 0, 0, 0) != 0) {
          fprintf(stderr, "%s\n", luaL_checkstring(l, -1));
          return false;
        }

        PostMessage(pp::Var("Lua loaded"));
        return true;
      }

      virtual ~AromaInstance() { }

      void HandleMessage(const pp::Var& var) {
        lua_getglobal(l, "nacl");
        lua_getfield(l, -1, "handle_message");
        if (lua_isnil(l, -1)) {
          fprintf(stderr, "Ignoring message, missing `nacl.handle_message`");
          return;
        }
        push_var(l, var);
        lua_call(l, 1, 0);
      }

      void DidChangeView(const pp::Rect& pos, const pp::Rect& clip) {
        PostMessage(pp::Var("DidChangeView"));
      }

      lua_State* lua() {
        return l;
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
