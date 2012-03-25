
#pragma once

#include "common.h"

namespace aroma {
	struct Image {
		GLuint texid;
		int width, height;

		void bind() const;

		static Image from_bytes(const byte* bytes, int width, int height, GLenum
				format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

		static int _gc(lua_State *l);

		static int _getWidth(lua_State *l);
		static int _getHeight(lua_State *l);

		// only push once, or add refernce counting so gc doesn't remove texture
		// being used by other images
		void push(lua_State *l) const;
	};
}

