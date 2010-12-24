#include "common.h"
// #include "window.h"
#include "canvas.h"

extern "C" {
	LUALIB_API int luaopen_aroma(lua_State *l) {
		lua_register(l, "aroma", Canvas::_new);
		lua_getglobal(l, "aroma");
		return 1;
	}
}

// read an integer array from table on top of stack
void readIntArray(lua_State *l, int *array, int count) {
	int buffsize = 10; // read 10 at a time

	int k = 0;
	while (count != 0) {
		int take = count < buffsize ? count : buffsize;
		for (int i = 0; i < take; i++) {
			lua_rawgeti(l, -(i+1), k+1);
			array[k++] = luaL_checkinteger(l, -1);
		}

		lua_pop(l, take);
		count -= take;
	}
}

void readArray(lua_State *l, double *array, int count) {
	int buffsize = 10; // read 10 at a time

	int k = 0;
	while (count != 0) {
		int take = count < buffsize ? count : buffsize;
		for (int i = 0; i < take; i++) {
			lua_rawgeti(l, -(i+1), k+1);
			array[k++] = luaL_checknumber(l, -1);
		}

		lua_pop(l, take);
		count -= take;
	}
}

// got this someplace off lua wiki/mailing list
void stackDump(lua_State *L) {
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


