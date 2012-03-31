
#include "lib/nacl.lua.h"

#include "nacl/aroma.h"
#include "nacl/gl_context.h"
#include "nacl/image.h"
#include "nacl/nacl_audio.h"

#include "lua_binding.h"
#include "renderer.h"
#include "input.h"

namespace aroma {

	byte* decode_byte_string(const char *str, size_t str_len, size_t num_bytes) {
		byte* bytes = new byte[num_bytes];
		size_t bi = 0;
		for (size_t i = 0; i < str_len; i++) {
			byte b = str[i];
			if (b < 128) {
				bytes[bi] = b;
			} else {
				byte b2 = str[++i];
				if (b == 194) {
					bytes[bi] = b2;
				} else {
					bytes[bi] = b2 + 64;
				}
			}
			bi++;
		}

		return bytes;
	}

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
		pp::Instance *instance  = upvalue_self(pp::Instance);
		instance->PostMessage(to_var(l, 1));
		return 0;
	}

	int _sleep(lua_State *l) {
		sleep(luaL_checknumber(l, 1));
		return 0;
	}

	int _time_ticks(lua_State *l) {
		pp::Core* core = pp::Module::Get()->core();
		lua_pushnumber(l, core->GetTimeTicks());
		return 1;
	}

	int _image_data_from_byte_string(lua_State *l) {
		size_t str_len;
		const char* str = lua_tolstring(l, 1, &str_len);
		int width = luaL_checknumber(l, 2);
		int height = luaL_checknumber(l, 3);

		int num_bytes = width*height*4;
		byte* bytes = decode_byte_string(str, str_len, num_bytes);

		return ImageData(width, height, bytes).push(l);
	}

	int _set_game_thread(lua_State *l);

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
				bind_function("time_ticks", _time_ticks);
				bind_function("image_data_from_byte_string", _image_data_from_byte_string);
				bind_function("set_game_thread", _set_game_thread);

				lua_settop(l, 0);

				if (!load_and_run(nacl_lua, nacl_lua_len, "nacl.lua")) {
					return false;
				}

				if (!LuaBinding::bind_all()) return false;

				lua_getglobal(l, "nacl");
				lua_getfield(l, -1, "init_all");
				if (!lua_isnil(l, -1)) {
					push_self();
					if (lua_pcall(l, 1, 0, 0) != 0) {
						err("%s\n", luaL_checkstring(l, -1));
						return false;
					}
				}

				return true;
			}

			void bind_module(Bindable *b) {
				LuaBinding::bind_module(b);

				// run any lua initialization if it exists
				int i = lua_gettop(l);
				lua_getglobal(l, "nacl");
				lua_getfield(l, -1, "init");
				if (lua_istable(l, -1)) {
					const char* module_name = b->module_name();
					lua_getfield(l, -1, module_name);
					if (!lua_isnil(l, -1)) {
						push_self();
						lua_getfield(l, -1, module_name);
						push_self();
						lua_remove(l, -3);

						if (lua_pcall(l, 2, 0, 0) != 0) {
							err("%s\n", luaL_checkstring(l, -1));
						}
					}
				}

				lua_settop(l, i);
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
					err("%s\n", luaL_checkstring(l, -1));
					return false;
				}

				return true;
			}

			void handle_error(lua_State *thread, const char* name) {
				LuaBinding::handle_error(thread, name);

				// convert msg to traceback
				int top = lua_gettop(thread);
				lua_getglobal(thread, "debug");
				lua_getfield(thread, -1, "traceback");
				lua_pushthread(thread);
				lua_pushvalue(thread, top);
				lua_call(thread, 2, 1); // :)

				lua_getglobal(l, "nacl");
				lua_getfield(l, -1, "show_error");
				if (!lua_isnil(l, -1)) {
					lua_xmove(thread, l, 1);

					if (lua_pcall(l, 1, 0, 0) != 0) {
						err("%s\n", luaL_checkstring(l, -1));
						return;
					}
				}

				lua_settop(thread, top);
			}
	};

	class AromaInstance : public pp::Instance {
		protected:
			NaClLuaBinding* binding;
			Renderer* renderer;
			InputHandler* input_handler;

		public:
			AromaInstance(PP_Instance instance) :
				pp::Instance(instance),
				renderer(NULL)
			{
				if (PP_OK != RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_KEYBOARD)) {
					log("Could not register input events\n");
				}
			}

			LuaBinding* get_binding() {
				return binding;
			}

			bool Init(uint32_t argc, const char** argn, const char** argv) {
				binding = new NaClLuaBinding(this);
				if (!binding->bind_all()) {
					log("Failed to init lua\n");
					return false;
				}

				input_handler = new InputHandler(binding);
				binding->bind_module(&ImageModule());
				binding->bind_module(&NaClAudioModule());

				PostMessage(pp::Var("Lua loaded"));
				return true;
			}

			virtual ~AromaInstance() {
				log("destructing instance");
				delete renderer;
				delete input_handler;
			}

			void HandleMessage(const pp::Var& var) {
				binding->handle_message(var);
			}

			bool HandleInputEvent(const pp::InputEvent& event) {
				if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEUP) {
					return true;
				}

				if (event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN) {
					pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
					input_handler->key_down(key.GetKeyCode());
					return true;
				} else if (event.GetType() == PP_INPUTEVENT_TYPE_KEYUP) {
					pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
					input_handler->key_up(key.GetKeyCode());
					return true;
				}

				return false;
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

	int _set_game_thread(lua_State *l) {
		upvalue_self(AromaInstance)->get_binding()->set_game_thread(lua_tothread(l, 1));
		return 0;
	}

}

namespace pp {
	Module* CreateModule() {
		return new aroma::AromaModule();
	}
}
