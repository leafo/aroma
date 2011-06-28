#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <string.h>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#define GL_GLEXT_PROTOTYPES
#include <GL/glfw.h>

#include <iostream>
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

#define $(val) #val ": " << val << " "

// void stackDump(lua_State *L);
void readIntArray(lua_State *l, int *array, int count);
void readArray(lua_State *l, double *array, int count);

// got this from someplace on lua mailing list
void stackDump(lua_State *L);

namespace aroma {
	extern const char *aroma_current_dir;
}

#endif /* COMMON_H_ */

