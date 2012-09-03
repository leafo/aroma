
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

#ifndef AROMA_NACL

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
#include <corona.h>
#endif

#endif

#include "image.h"

// Warning: osx represents pixels at BGRA, watch out!

namespace aroma {
	const GLenum IMAGE_DATA_FORMAT = GL_RGBA;
	const GLenum IMAGE_DATA_TYPE = GL_UNSIGNED_BYTE;

	int next_p2(int x) {
		int size = 1;
		while (size < x) {
			size = size << 1;
		}
		return size;
	}

	// platform specific loaders
#ifndef AROMA_NACL

#ifdef __APPLE__

	CGImageSourceRef source_from_bytes(const void* bytes, size_t len);
	CGImageSourceRef source_from_fname(const char* fname);
	void copy_image(CGImageSourceRef source, ImageData* data);

#else

	void copy_image(corona::Image *src, ImageData* dest);

#endif

#endif

	ImageData::ImageData(int width, int height, byte* bytes)
		: width(width), height(height), bytes(bytes), format(IMAGE_DATA_FORMAT),
		type(IMAGE_DATA_TYPE) {}

	ImageData::ImageData()
		: width(0), height(0), bytes(0), format(IMAGE_DATA_FORMAT),
		type(IMAGE_DATA_TYPE) {}

	ImageData::ImageData(int width, int height)
		: width(width), height(height), format(IMAGE_DATA_FORMAT),
		type(IMAGE_DATA_TYPE)
	{
		bytes = new byte[width*height*4];
	}

	void ImageData::free() {
		delete bytes;
		bytes = 0;
	}

	void ImageData::clear(const Color color) {
		Color* pixels = (Color*)bytes;
		int count = width * height;
		for (int i = 0; i < count; i++) {
			pixels[i] = color;
		}
	}

	int ImageData::_gc(lua_State* l) {
		getself(ImageData)->free();
		return 0;
	}

	int ImageData::_getWidth(lua_State* l) {
		lua_pushnumber(l, getself(ImageData)->width);
		return 1;
	}

	int ImageData::_getHeight(lua_State* l) {
		lua_pushnumber(l, getself(ImageData)->height);
		return 1;
	}

	int ImageData::_getPixel(lua_State* l) {
		ImageData* d = getself(ImageData);
		Color *pixels = (Color*)d->bytes;

		Point p = Point::read2d(l, 2);
		return pixels[(int)p.y * d->width + (int)p.x].push(l);
	}

	int ImageData::_setPixel(lua_State* l) {
		ImageData* d = getself(ImageData);
		Point p = Point::read2d(l, 2);
		Color c = Color::read(l, 4);

		Color *pixels = (Color*)d->bytes;
		pixels[(int)p.y * d->width + (int)p.x] = c;
		return 0;
	}

	int ImageData::_new(lua_State* l) {
		Point p = Point::read2d(l, 1);
		int x = p.x;
		int y = p.y;

		return ImageData(x, y, new byte[x*y*4]).push(l);
	}

	int ImageData::push(lua_State* l) const {
		ImageData* d = newuserdata(ImageData);
		*d = *this;

		if (luaL_newmetatable(l, "ImageData")) {
			lua_newtable(l); // the index table
			setfunction("getWidth", _getWidth);
			setfunction("getHeight", _getHeight);
			setfunction("getPixel", _getPixel);
			setfunction("setPixel", _setPixel);

			lua_setfield(l, -2, "__index");

			setfunction("__gc", _gc);
		}

		lua_setmetatable(l, -2);

		return 1;
	}

	void ImageData::update(int x, int y, const ImageData & other) {
		Color* source = (Color*)other.bytes;
		Color* dest = (Color*)bytes;

		for (int i = 0; i < other.width * other.height; i++) {
			int xx = x + (i % other.width);
			int yy = y + (i / other.width);
			dest[xx + yy * width] = source[i];
		}
	}

	ImageData ImageData::slice(int x, int y, int w, int h) {
		ImageData out = ImageData(w, h);

		int dest_i = 0;
		int my = y + h;

		Color* source_pixels = (Color*)bytes;
		Color* dest_pixels = (Color*)out.bytes;

		for (int yy = y; yy < my; yy++) {
			int source_i = width * yy + x;
			int end_i = source_i + w;
			while (source_i < end_i) {
				dest_pixels[dest_i] = source_pixels[source_i];

				dest_i++;
				source_i++;
			}
		}

		return out;
	}

	void ImageData::apply_color_key(const Color key) {
		Color* pixels = (Color*)bytes;
		for (int i = 0; i < width*height; i++) {
			if (*pixels == key) pixels->a = 0;
			pixels++;
		}
	}

#ifndef AROMA_NACL
	bool from_memory_file(ImageData* data, const void* bytes, size_t len) {
#ifdef __APPLE_
		CGImageSourceRef source = source_from_bytes(bytes, len);
		if (!source) return false;
		copy_image(source, data);
		CFRelease(source);
#else
	corona::Image *tmp = corona::OpenImage(corona::CreateMemoryFile(bytes, len),
			corona::PF_R8G8B8A8);

	if (!tmp) return false; // failed
	copy_image(tmp, data);
	delete tmp;
#endif
		return true;
	}

	bool from_file(ImageData *data, const char* fname) {
#ifdef __APPLE_
		CGImageSourceRef source = source_from_fname(fname);
		if (!source) return false;
		copy_image(source, data);
		CFRelease(source);
#else
		corona::Image *tmp = corona::OpenImage(fname, corona::PF_R8G8B8A8);
		if (!tmp) return false; // failed
		copy_image(tmp, data);
		delete tmp;
#endif
		return true;
	}
#endif

	GLenum get_wrap_mode(const char* name) {
		if (strcmp(name, "repeat") == 0) {
			return GL_REPEAT;
		} else if (strcmp(name, "clamp") == 0) {
			return GL_CLAMP_TO_EDGE;
		}
		return -1;
	}

	GLenum get_filter(const char* name) {
		if (strcmp(name, "linear") == 0) {
			return GL_LINEAR;
		} else if (strcmp(name, "nearest") == 0) {
			return GL_NEAREST;
		}
		return -1;
	}


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

	Image Image::from_data(const ImageData & data) {
		return from_bytes(data.bytes, data.width, data.height,
				data.format, data.type);
	}

	void Image::bind() const {
		glBindTexture(GL_TEXTURE_2D, texid);
	}

	void Image::update(int x, int y, const ImageData & data) {
		bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, data.width, data.height,
				data.format, data.type, data.bytes);
	}

	void Image::free() {
		glDeleteTextures(1, &texid);
		texid = -1;
	}

	int Image::_gc(lua_State* l) {
		Image* self = newuserdata(Image);
		self->free();
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
				return luaL_error(l, "unknown horizontal wrap mode: %s",
						luaL_checkstring(l, 2));
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
		}

		if (top > 2 && !lua_isnil(l, 3)) {
			GLenum mode = get_wrap_mode(luaL_checkstring(l, 3));
			if (mode < 0) {
				return luaL_error(l, "unknown vertical wrap mode: %s",
						luaL_checkstring(l, 3));
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
		}

		return 0;
	}

	int Image::_setFilter(lua_State* l) {
		Image* self = getself(Image);
		int top = lua_gettop(l);

		if (top > 1 && !lua_isnil(l, 2)) {
			GLenum filter = get_filter(luaL_checkstring(l, 2));
			if (filter < 0) {
				return luaL_error(l, "unknown min filter mode: %s",
						luaL_checkstring(l, 2));
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		}

		if (top > 2 && !lua_isnil(l, 3)) {
			GLenum filter = get_filter(luaL_checkstring(l, 3));
			if (filter < 0) {
				return luaL_error(l, "unknown mag filter mode: %s",
						luaL_checkstring(l, 3));
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		}


		glBindTexture(GL_TEXTURE_2D, self->texid);

		return 0;
	}

	// take image data as first arguments
	int Image::_new(lua_State* l) {
		Image new_img = from_data(*getself(ImageData));

		Image *self = newuserdata(Image);
		*self = new_img;

		if (luaL_newmetatable(l, "Image")) {
			lua_newtable(l); // the index table
			setfunction("getWidth", _getWidth);
			setfunction("getHeight", _getHeight);
			setfunction("setWrap", _setWrap);
			setfunction("setFilter", _setFilter);

			lua_setfield(l, -2, "__index");

			setfunction("__gc", _gc);
		}

		lua_setmetatable(l, -2);

		return 1;
	}

	const char* ImageModule::module_name() {
		return "image";
	}

	void ImageModule::bind_all(lua_State* l) {
		setfunction("newImageData", ImageData::_new);
	}

#ifndef AROMA_NACL

#ifdef __APPLE__
	//	TODO: I don't even know if this code compiles :)

	CGImageSourceRef source_from_bytes(const void* bytes, size_t len) {
		CFDataRef data = CFDataCreate(NULL, (UInt8*)bytes, len);
		CGImageSourceRef source = CGImageSourceCreateWithData(data, NULL);
		CFRelease(data);
		return source;
	}

	CGImageSourceRef source_from_fname(const char* fname) {
		CFStringRef path_str = CFStringCreateWithCString(0, fname, 0);

		CFURLRef path = 0;
		path = CFURLCreateWithFileSystemPath(NULL, path_str,
				kCFURLPOSIXPathStyle, false);

		// CFShow(path);

		CGImageSourceRef source = CGImageSourceCreateWithURL(path, NULL);

		CFRelease(path);
		CFRelease(path_str);
		return source;
	}

	void copy_image(CGImageSourceRef source, ImageData* data) {
		int count = CGImageSourceGetCount(source);
		// CGImageSourceStatus status = CGImageSourceGetStatus(source);
		// printf("status: %d\n", status);

		CGImageRef image = CGImageSourceCreateImageAtIndex(source, 0, NULL);

		int width, height;
		width = CGImageGetWidth(image);
		height = CGImageGetHeight(image);

		GLubyte *buffer = new GLubyte[width * height * 4];
		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();

		int bytes_per_row = width * 4;

		CGContextRef context = CGBitmapContextCreate(buffer, width, height, 8,
				bytes_per_row, color_space, kCGImageAlphaPremultipliedFirst);
		CGContextDrawImage(context, CGRectMake(0,0, width, height), image);

		data->width = width;
		data->height = height;
		data->format = GL_BGRA;
		data->type = GL_UNSIGNED_INT_8_8_8_8;
		data->bytes = buffer;

		CGContextRelease(context);
		CFRelease(color_space);

		CGImageRelease(image);
	}

#else

	void copy_image(corona::Image *src, ImageData* dest) {
		dest->width = src->getWidth();
		dest->height = src->getHeight();
		dest->bytes = new byte[dest->width * dest->height * 4];

		Color* dest_pixels = (Color*)dest->bytes;
		Color* source_pixels = (Color*)src->getPixels();

		int size = dest->width * dest->height;
		for (int i = 0; i < size; i++) {
			dest_pixels[i] = source_pixels[i];
		}
	}

#endif

#endif
}

