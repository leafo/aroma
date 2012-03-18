
#include "nacl.lua.h"

#include "nacl/aroma.h"
#include "nacl/gl_context.h"
#include "lua_binding.h"
#include "renderer.h"

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

	int _sleep(lua_State *l) {
		sleep(luaL_checknumber(l, 1));
		return 0;
	}

	void sleep(float seconds) {
		long nanoseconds = (long)(seconds * 1000000000);
		timespec req = { 0, nanoseconds };
		nanosleep(&req, NULL);
	}

	class NaClLuaBinding : public LuaBinding {
		protected:
			pp::Instance* instance;

			void bind_function(const char* name, lua_CFunction func) {
				lua_getglobal(l, "nacl");
				lua_pushlightuserdata(l, instance);
				lua_pushcclosure(l, func, 1);
				lua_setfield(l, -2, name);
				lua_pop(l, 1);
			}

		public:
			NaClLuaBinding(pp::Instance* instance) : instance(instance) { }

			bool bind_all() {
				log("binding nacl to lua\n");
				luaopen_cjson(l);
				preload_library("cjson");

				lua_newtable(l);
				lua_setglobal(l, "nacl");

				bind_function("post_message", _post_message);
				bind_function("sleep", _sleep);

				lua_settop(l, 0);

				if (luaL_loadbuffer(l, (const char*)lib_nacl_lua, lib_nacl_lua_len, "nacl.lua") != 0) {
					log("%s\n", luaL_checkstring(l, -1));
					return false;
				}

				if (lua_pcall(l, 0, 0, 0) != 0) {
					log("%s\n", luaL_checkstring(l, -1));
					return false;
				}

				return LuaBinding::bind_all();
			}

			bool handle_message(const pp::Var& var) {
				lua_getglobal(l, "nacl");
				lua_getfield(l, -1, "handle_message");
				if (lua_isnil(l, -1)) {
					log("Ignoring message, missing `nacl.handle_message`\n");
					return false;
				}
				push_var(l, var);

				if (lua_pcall(l, 1, 0, 0) != 0) {
					log("%s\n", luaL_checkstring(l, -1));
					return false;
				}

				return true;
			}
	};

	class AromaInstance : public pp::Instance {
		protected:
			NaClLuaBinding* binding;
			Renderer* renderer;

		public:
			AromaInstance(PP_Instance instance) :
				pp::Instance(instance),
				renderer(NULL) { }

			bool Init(uint32_t argc, const char** argn, const char** argv) {
				binding = new NaClLuaBinding(this);
				if (!binding->bind_all()) {
					log("Failed to init lua\n");
					return false;
				}

				PostMessage(pp::Var("Lua loaded"));
				return true;
			}

			virtual ~AromaInstance() { }

			void HandleMessage(const pp::Var& var) {
				binding->handle_message(var);
			}

			void DidChangeView(const pp::Rect& pos, const pp::Rect& clip) {
				// PostMessage(pp::Var("DidChangeView"));
				log("didchangeview\n");
				if (!renderer) {
					renderer = new Renderer(new OpenGLContext(this), binding);
				}

				pp::Size size = pos.size();
				renderer->reshape(size.width(), size.height());
			}
	};

	class AromaModule : public pp::Module {
		public:
			AromaModule() : pp::Module() {}
			virtual ~AromaModule() {}

			virtual bool Init() {
				return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
			}

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
