
#pragma once

#include "common.h"

namespace aroma {
	struct Image {
		GLuint texid;
		int width, height;

		static Image from_bytes(byte* bytes, int w, int h, int channels);
		static int _getWidth(lua_State *l);
		static int _getHeight(lua_State *l);

		void push(lua_State *l);
	};
}

