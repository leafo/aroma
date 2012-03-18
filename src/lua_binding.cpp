
#include "lua_binding.h"
#include "renderer.h"

namespace aroma {
	static const char* aroma_name = "aroma";

	LuaBinding::LuaBinding() {
		l = luaL_newstate();
		luaL_openlibs(l);
		log("created a lua\n");
	}

	bool LuaBinding::bind_all() {
		lua_newtable(l);
		lua_setglobal(l, aroma_name);
		return true;
	}

	void LuaBinding::bind_module(Bindable *b) {
		lua_getglobal(l, aroma_name);
		lua_newtable(l);
		int i = lua_gettop(l);
		b->bind_all(l);
		lua_settop(l, i);
		lua_setfield(l, -2, b->module_name());
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

