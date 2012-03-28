
#include "nacl/image.h"

namespace aroma {

	GLenum get_wrap_mode(const char* name) {
		if (strcmp(name, "repeat") == 0) {
			return GL_REPEAT;
		} else if (strcmp(name, "clamp") == 0) {
			return GL_CLAMP_TO_EDGE;
		}
		return -1;
	}

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

	void Image::bind() const {
		glBindTexture(GL_TEXTURE_2D, texid);
	}

	int Image::_gc(lua_State* l) {
		Image* self = newuserdata(Image);
		glDeleteTextures(1, &self->texid);
		self->texid = -1;
		return 0;
	}

	int Image::_getWidth(lua_State* l) {
		lua_pushnumber(l, getself(Image)->width);
		return 1;
	}

	int Image::_getHeight(lua_State* l) {
		lua_pushnumber(l, getself(Image)->height);
		return 1;
	}

	int Image::_setWrap(lua_State* l) {
		Image* self = getself(Image);
		int top = lua_gettop(l);

		glBindTexture(GL_TEXTURE_2D, self->texid);

		if (top > 1 && !lua_isnil(l, 2)) {
			GLenum mode = get_wrap_mode(luaL_checkstring(l, 2));
			if (mode < 0) {
				return luaL_error(l, "unknown horizontal wrap mode: %s", luaL_checkstring(l, 2));
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
		}

		if (top > 2 && !lua_isnil(l, 3)) {
			GLenum mode = get_wrap_mode(luaL_checkstring(l, 3));
			if (mode < 0) {
				return luaL_error(l, "unknown vertical wrap mode: %s", luaL_checkstring(l, 3));
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
		}

		return 0;
	}

	void Image::push(lua_State *l) const {
		Image *self = newuserdata(Image);

		if (luaL_newmetatable(l, "Image")) {
			lua_newtable(l); // the index table
			setfunction("getWidth", _getWidth);
			setfunction("getHeight", _getHeight);
			setfunction("setWrap", _setWrap);

			lua_setfield(l, -2, "__index");

			setfunction("__gc", _gc);
		}

		lua_setmetatable(l, -2);

		*self = *this;
	}

}

