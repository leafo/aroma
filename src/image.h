
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

		ImageData(int width, int height, byte* bytes); // takes ownership of buffer
		ImageData(int width, int height); // creates the buffer
		ImageData();

		void free(); // don't call if you've handed it off to lua
		void clear(const Color color);

		void update(int x, int y, const ImageData & other);
		void apply_color_key(const Color key);

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
		void update(int x, int y, const ImageData & data);

		void free();

		static Image from_bytes(const byte* bytes, int width, int height, GLenum
				format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

		static Image from_data(const ImageData & data);

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

