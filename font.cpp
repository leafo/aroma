#include <string.h>
#include "common.h"

void Font::string(Point dest, const char *str)
{
	realwidth = 7;
	realheight = th;

	dest.x -= (tw - realwidth) / 2;
	Point origin = dest;

	int len = strlen(str);
	while (len > 0)
	{
		switch (*str) {
		case '\n':
			str++;
			dest.y += realheight;
			dest.x = origin.x;
			break;
		default:
			tile(dest, *str++);	// put letter
			dest.x += realwidth;
		}
		len--;
	}
}

void Font::install(lua_State *l)
{
	luaL_newmetatable(l, "Font");
	setfunction("__call", Font::_string);
}

Font Font::pop(lua_State *l)
{
	Font *f = (Font*)luaL_checkudata(l, -1, "Font");
	lua_pop(l, 1);
	return *f;
}

int Font::_new(lua_State *l)
{
	Point lsize = Point::pop(l);
	const char *fname = luaL_checkstring(l, -1);
	
	Font & f = *(Font*)lua_newuserdata(l, sizeof(Font));

	if (!f.load(fname, lsize.x, lsize.y))
		return luaL_error(l, "Failed to load font");

	if (luaL_newmetatable(l, "Font")) {
		lua_pop(l, 1);
		Font::install(l);
	}

	lua_setmetatable(l, -2);

	return 1;
}

int Font::_string(lua_State *l)
{
	const char *str = luaL_checkstring(l, -1); lua_pop(l, 1);
	Point dest = Point::pop(l);
	Font f = Font::pop(l);
	f.string(dest, str);

	return 0;
}


