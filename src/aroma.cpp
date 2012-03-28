#include "common.h"
#include "canvas.h"
#include "image.h"

using namespace aroma;

namespace aroma {
	const char *aroma_current_dir;
}

static const struct luaL_Reg aroma_funcs [] = {
	{"new", Canvas::_new},
	{"image_bytes", Image::_get_image_bytes},
	{NULL, NULL}
};

extern "C" {
	LUALIB_API int luaopen_aroma(lua_State *l) {
		aroma_current_dir = getcwd(NULL, 0);
		luaL_register(l, "aroma", aroma_funcs);

		if (luaL_newmetatable(l, "aroma")) {
			// alias for aroma.new
			setfunction("__call", Canvas::_new);
		}

		lua_setmetatable(l, -2);
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


