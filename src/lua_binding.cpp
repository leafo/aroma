
#include "lua_binding.h"

namespace aroma {

	LuaBinding::LuaBinding() {
		l = luaL_newstate();
		luaL_openlibs(l);
		log("created a lua\n");
	}

	bool LuaBinding::bind_all() {
		log("suck my pisser!\n");
		return true;
	}

	lua_State* LuaBinding::lua() {
		return l;
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
}

