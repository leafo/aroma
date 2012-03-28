
#pragma once

#include "common.h"
#include "lua_binding.h"

namespace aroma {
	struct Image {
		GLuint texid;
		int width, height;

		void bind() const;

		static Image from_bytes(const byte* bytes, int width, int height, GLenum
				format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

		static int _gc(lua_State* l);

		static int _getWidth(lua_State* l);
		static int _getHeight(lua_State* l);

		static int _setWrap(lua_State* l);

		static int _new(lua_State* l);
	};

	struct ImageData {
		int width, height;
		byte* bytes;

		// TODO don't need constructor
		ImageData(int width, int height, byte* bytes);

		static int _gc(lua_State* l);
		static int _getWidth(lua_State* l);
		static int _getHeight(lua_State* l);

		static int _getPixel(lua_State* l);
		static int _setPixel(lua_State* l);

		static int _new(lua_State* l);

		int push(lua_State* l) const;
	};

	class ImageModule : public Bindable {
		const char* module_name();
		void bind_all(lua_State* l);
	};
}

