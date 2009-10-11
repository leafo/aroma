/**
 * game engine
 * start: Thu Jul 30 11:29:55 EDT 2009
 */

// need a pop rect
// need to change points to use x and y instead of nums?
// make point not crash the engine

#include "common.h"

// am I going to use these?
byte Color::MAX = 255;
Color Color::White = Color(255,255,255);
Color Color::Black = Color(0,0,0);
Color Color::Red = Color(255,0,0);
Color Color::Green = Color(0,255,0);
Color Color::Blue = Color(0,0,255);
Color Color::Gray = Color(200,200,200);

double Color::rf() {
	return 1.0 * r / MAX;
}

double Color::gf() {
	return 1.0 * g / MAX;
}

double Color::bf() {
	return 1.0 * b / MAX;
}

int main(int argc, char *argv[])
{
	lua_State *l = lua_open();

	luaL_openlibs(l);
	Window::install(l);
	Point::install(l);

	if (luaL_dofile(l, argc > 1 ? argv[1] : "first.lua")) {
		// something failed
		fprintf(stderr, "%s\n", lua_tostring(l, -1));
		lua_pop(l, 1);
	}

	/*
	// lets try to call the function
	lua_getglobal(l, "Color");
	lua_pushinteger(l, 175);
	lua_pushinteger(l, 255);
	lua_pushinteger(l, 255);

	if(lua_pcall(l, 3, 1, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(l, -1));
		lua_pop(l, 1);
	}
	*/

	lua_close(l);

	return 0;
}


void stackDump (lua_State *L) 
{
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
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
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}



