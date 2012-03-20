
#include "nacl/image.h"

namespace aroma {

	struct Pixel {
		byte r, g, b, a;

		Pixel() {}
		Pixel(byte r, byte g, byte b) : r(r), g(g), b(b), a(255) { }

		bool operator==(const Pixel &other) const {
			return r == other.r &&g == other.g && b == other.b;
		}
	};

	Image Image::from_bytes(const byte* bytes, int w, int h, GLenum format, GLenum type) {
		Image img = {0};
		img.width = w;
		img.height = h;

		glGenTextures(1, &img.texid);
		glBindTexture(GL_TEXTURE_2D, img.texid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, type, bytes);

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

