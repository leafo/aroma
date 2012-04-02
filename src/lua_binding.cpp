
#include "lua_binding.h"
#include "renderer.h"

namespace aroma {
	static const char* AROMA_NAME = "aroma";

	LuaBinding::LuaBinding() : game_thread(NULL) {
		l = luaL_newstate();
		luaL_openlibs(l);
		log("created a lua\n");
	}

	bool LuaBinding::bind_all() {
		lua_newtable(l);
		lua_setglobal(l, AROMA_NAME);

		return true;
	}

	void LuaBinding::bind_module(Bindable *b) {
		lua_getglobal(l, AROMA_NAME);
		lua_newtable(l);
		int i = lua_gettop(l);
		b->bind_all(l);
		lua_settop(l, i);
		lua_setfield(l, -2, b->module_name());
	}

	lua_State* LuaBinding::lua() {
		return l;
	}

	void LuaBinding::push_self() {
		lua_getglobal(l, AROMA_NAME);
	}

	// takes value on top of stack and puts it in package.loaded[name]
	// pops value
	void LuaBinding::preload_library(const char* name) {
		int i = lua_gettop(l);
		lua_getglobal(l, "package");
		lua_getfield(l, -1, "loaded");
		lua_pushvalue(l, i);
		lua_setfield(l, -2, name);
		lua_settop(l, i - 1);
	}

	void LuaBinding::set_game_thread(lua_State* thread) {
		game_thread = thread;
	}

	void LuaBinding::send_event(const char* name, int nargs) {
		if (!game_thread) return;

		lua_pushstring(game_thread, name);
		lua_xmove(l, game_thread, nargs);
		int status = lua_resume(game_thread, nargs + 1);

		if (status != 0 && status != LUA_YIELD) {
			handle_error(game_thread, name);
			game_thread = NULL;
			return;
		}

		lua_settop(game_thread, 0); // don't care!
	}

	void LuaBinding::handle_error(lua_State *thread, const char* name) {
		err("event `%s` failed:\n", name);
		stack_dump(thread);
	}

	bool LuaBinding::load_and_run(void* buffer, size_t buff_len, const char* name) {
		if (luaL_loadbuffer(l, (const char*)buffer, buff_len, name) != 0) {
			err("%s\n", luaL_checkstring(l, -1));
			return false;
		}

		if (lua_pcall(l, 0, 0, 0) != 0) {
			err("%s\n", luaL_checkstring(l, -1));
			return false;
		}

		return true;
	}

	// check if the ith item on the stack has metatable called type
	bool LuaBinding::is_type(lua_State* l, int i, const char* type) {
		int top = lua_gettop(l);
		lua_getmetatable(l, i);
		lua_getfield(l, LUA_REGISTRYINDEX, type);
		bool eq  = (bool)lua_rawequal(l, -1, -2);
		lua_settop(l, top);
		return eq;
	}

	// store something in the registry to prevent it from being garbage collected
	void LuaBinding::store_in_registry(lua_State *l, int i, const char* name) {
		luaL_newmetatable(l, AROMA_NAME);
		lua_pushvalue(l, 1);
		lua_setfield(l, -2, name);
		lua_pop(l, 1); // pop metatable
	}

	// get something stored in registry
	int LuaBinding::from_registry(lua_State *l, const char* name) {
		luaL_newmetatable(l, AROMA_NAME);
		lua_getfield(l, -1, name);
		lua_remove(l, -2);
		return 1;
	}

	// got this someplace off lua wiki/mailing list
	void stack_dump(lua_State *L) {
		int i;
		int top = lua_gettop(L);
		for (i = 1; i <= top; i++) {  /* repeat for each level */
			printf("%d: ", i);
			int t = lua_type(L, i);
			switch (t) {
				case LUA_TSTRING:  /* strings */
					printf("`%s'", lua_tostring(L, i));
					break;

				case LUA_TBOOLEAN:  /* booleans */
					printf(lua_toboolean(L, i) ? "true" : "false");
					break;

				case LUA_TNUMBER:  /* numbers */
					printf("%g", lua_tonumber(L, i));
					break;

				default:  /* other values */
					printf("%s", lua_typename(L, t));
					break;

			}
			printf("\n");  /* put a separator */
		}
		printf("\n");  /* end the listing */
	}

}

