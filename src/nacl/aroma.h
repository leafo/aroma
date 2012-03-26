
#pragma once

#include <cstdio>
#include <string>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "ppapi/cpp/rect.h"

#include "ppapi/cpp/input_event.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <sys/time.h>
#include <sys/nacl_syscalls.h>

#include "common.h"

using namespace std;

extern "C" int luaopen_cjson(lua_State *l);

namespace aroma {
	byte* decode_byte_string(const char* str, size_t str_len, size_t num_bytes);

	void push_var(lua_State* l, pp::Var var);
	pp::Var to_var(lua_State* l, int index);
}


