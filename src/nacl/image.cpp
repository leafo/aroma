
#include "nacl/image.h"

namespace aroma {

	Image Image::from_bytes(byte* bytes, int w, int h, int channels) {
		Image img = {0};
		img.width = w;
		img.height = h;
		return img;
	}

	int Image::_getWidth(lua_State *l) {
		lua_pushnumber(l, getself(Image)->width);
		return 1;
	}

	int Image::_getHeight(lua_State *l) {
		lua_pushnumber(l, getself(Image)->height);
		return 1;
	}

	void Image::push(lua_State *l) {
		Image *self = newuserdata(Image);

		if (luaL_newmetatable(l, "Image")) {
			lua_newtable(l); // the index table
			setfunction("getWidth", _getWidth);
			setfunction("getHeight", _getHeight);

			lua_setfield(l, -2, "__index");
		}

		lua_setmetatable(l, -2);

		*self = *this;
	}

}

