#pragma once

#include <stdio.h>
#include <string.h>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#ifdef AROMA_NACL
#include "nacl/gl.h"
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glfw.h>
#include <iostream>
#endif

using namespace std;

typedef unsigned char byte;

typedef void(*AromaRegister)(lua_State *l);

#define setint(name,val) lua_pushinteger(l, val);\
	lua_setfield(l, -2, name)

#define setnumber(name,val) lua_pushnumber(l, val);\
	lua_setfield(l, -2, name)

#define setbool(name,val) lua_pushboolean(l, val);\
	lua_setfield(l, -2, name)

#define setfunction(name,val) lua_pushcfunction(l, val);\
	lua_setfield(l, -2, name)

#define newuserdata(type) ((type*)lua_newuserdata(l, sizeof(type)))
#define getself(type) ((type*)luaL_checkudata(l, 1, #type))
#define getselfi(type, i) ((type*)luaL_checkudata(l, i, #type))

#define set_new_func(name, func) lua_pushlightuserdata(l, this);\
	lua_pushcclosure(l, func, 1);\
	lua_setfield(l, -2, name)

#define upvalue_self(type) ((type*)lua_touserdata(l, lua_upvalueindex(1)))

#define $(val) #val ": " << val << " "

#define __strx(x) #x
#define __str(x) __strx(x)
#define log(...) fprintf(stderr, " [" __FILE__  ":" __str(__LINE__) "] \033[1;33m>>\033[0m " __VA_ARGS__)
#define err(...) fprintf(stderr, " [" __FILE__  ":" __str(__LINE__) "] \033[1;31m>>\033[0m " __VA_ARGS__)

// void stackDump(lua_State *L);
void readIntArray(lua_State *l, int *array, int count);
void readArray(lua_State *l, double *array, int count);

// got this from someplace on lua mailing list
void stackDump(lua_State *L);

namespace aroma {
	extern const char *aroma_current_dir;
}

