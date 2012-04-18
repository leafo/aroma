#include "nacl/nacl_time.h"

namespace aroma {

	NaClTimeModule::NaClTimeModule(Renderer* renderer)
		: renderer(renderer) { }

	// this can be generic
	int NaClTimeModule::_getTime(lua_State* l) {
		NaClTimeModule* time = upvalue_self(NaClTimeModule);
		lua_pushnumber(l, time->renderer->get_context()->get_time());
		return 1;
	}

	int NaClTimeModule::_getFPS(lua_State* l) {
		NaClTimeModule* time = upvalue_self(NaClTimeModule);
		lua_pushnumber(l, time->renderer->get_fps());
		return 1;
	}

	const char* NaClTimeModule::module_name() {
		return "timer";
	}

	void NaClTimeModule::bind_all(lua_State* l) {
		set_new_func("getTime", _getTime);
		set_new_func("getFPS", _getFPS);
	}

}
