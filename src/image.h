
#pragma once

#include "common.h"
#include "geometry.h"
#include "lua_binding.h"

namespace aroma {
	int next_p2(int x);

	struct ImageData {
		GLenum format;
		GLenum type;

		int width, height;
		byte* bytes;

		void apply_color_key(const Color key);

		// TODO get rid of constructor
		ImageData(int width, int height, byte* bytes);
		ImageData();

#ifndef AROMA_NACL
		static bool from_memory_file(ImageData* d, const void* bytes, size_t len);
		static bool from_file(ImageData* d, const char* fname);
#endif

		static int _gc(lua_State* l);
		static int _getWidth(lua_State* l);
		static int _getHeight(lua_State* l);

		static int _getPixel(lua_State* l);
		static int _setPixel(lua_State* l);

		static int _new(lua_State* l);

		int push(lua_State* l) const;
	};

	struct Image {
		GLuint texid;
		int width, height;

		void bind() const;
		void update(int x, int y, ImageData* data);

		static Image from_bytes(const byte* bytes, int width, int height, GLenum
				format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

		static int _gc(lua_State* l);

		static int _getWidth(lua_State* l);
		static int _getHeight(lua_State* l);

		static int _setWrap(lua_State* l);

		static int _new(lua_State* l);
	};

	class ImageModule : public Bindable {
		const char* module_name();
		void bind_all(lua_State* l);
	};

}

