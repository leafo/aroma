#include "image.h"

#include <cstdlib>
#include <cstdio>
#include <unistd.h>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
#include <corona.h>
#endif

namespace aroma {

void register_Image(lua_State *l) {
	setfunction("image", Image::_new_from_file);
	setfunction("image_from_bytes", Image::_new_from_raw);
	setfunction("image_from_string", Image::_new_from_memory);
	setfunction("image_bytes", Image::_get_image_bytes);
}

static int texCoords[] = {
	0,0,  1,0,  1,1,  0,1
};

static int next_p2(int x);

struct Pixel {
	byte r, g, b, a;

	Pixel() {}
	Pixel(byte r, byte g, byte b) : r(r), g(g), b(b), a(255) { }

	bool operator==(const Pixel &other) const {
		return r == other.r &&g == other.g && b == other.b;
	}
};

/**
 * Create userdata for image
 */
static Image *push_image(lua_State *l) {
	Image *i = newuserdata(Image);

	if (luaL_newmetatable(l, "Image")) {
		lua_newtable(l); // the index table
		setfunction("draw", Image::_draw);
		setfunction("blit", Image::_blit);
		setfunction("bind", Image::_bind);
		setfunction("size", Image::_size);

		setfunction("raw_update", Image::_raw_update);

		lua_setfield(l, -2, "__index");
	}

	lua_setmetatable(l, -2);

	return i;
}


/**
 * Modify the image such that color key is transparent
 */
static void apply_color_key(int width, int height, Pixel *pixels, Pixel key) {
	for (int i = 0; i < width*height; i++) {
		if (*pixels == key) pixels->a = 0;
		pixels++;
	}
}

#ifdef __APPLE__

char *file_bytes(const char *fname, size_t *_size) {
	FILE *handle = fopen(fname, "rb");
	if (!handle) return 0;

	fseek(handle, 0, SEEK_END);
	size_t size = ftell(handle);
	rewind(handle);

	cout << "reading " << size << " bytes" << endl;
	char *buffer = new char[size];

	fread(buffer, 1, size, handle);
	*_size = size;
	return buffer;
}

CGImageSourceRef source_from_bytes(const void *bytes, size_t len) {
	CFDataRef data = CFDataCreate(NULL, (UInt8*)bytes, len);
	CGImageSourceRef source = CGImageSourceCreateWithData(data, NULL);
	CFRelease(data);
	return source;
}

CGImageSourceRef source_from_fname(const char *fname) {
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

// upload from cgsource
int copy_image(CGImageSourceRef source, Image *dest) {
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

	dest->create(width, height, buffer, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8);

	CGContextRelease(context);
	CFRelease(color_space);
	delete buffer;

	CGImageRelease(image);
	return 0;
}

#else

// upload from corona image
static bool copy_image(corona::Image *src, Image *dest) {
	apply_color_key(src->getWidth(), src->getHeight(),
			(Pixel*)src->getPixels(), Pixel(255, 0, 255));

	double start = glfwGetTime();
	dest->create(src->getWidth(), src->getHeight(), src->getPixels());
	cout << "uploading image: " << glfwGetTime() - start << endl;
	return true;
}

#endif

void Image::bind() const {
	glBindTexture(GL_TEXTURE_2D, texid);
}

void Image::create(int width, int height, const void *bytes, GLenum format, GLenum type) {
	this->width = width;
	this->height = height;

	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, type, bytes);
}

void Image::update(int x, int y, int width, int height, const void *bytes) {
	bind();
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height,
			GL_RGBA, GL_UNSIGNED_BYTE, bytes);
}

bool Image::load_memory(const void* bytes, int size) {
	double start = glfwGetTime();
#ifdef __APPLE__
	CGImageSourceRef source = source_from_bytes(bytes, size);
	if (!source) return false;
	copy_image(source, this);
	CFRelease(source);
#else
	corona::Image *tmp = corona::OpenImage(corona::CreateMemoryFile(bytes, size),
			corona::PF_R8G8B8A8);

	if (!tmp) return false; // failed

	copy_image(tmp, this);

	delete tmp;
#endif
	cout << "loading image: " << glfwGetTime() - start << endl;
	return true;
}

bool Image::load(const char *fname) {
	double start = glfwGetTime();
#ifdef __APPLE__
	// size_t size;
	// char *bytes = file_bytes(fname, &size);
	// if (!bytes) return false;

	// CGImageSourceRef source = source_from_bytes(bytes, size);
	
	CGImageSourceRef source = source_from_fname(fname);
	if (!source) return false;
	copy_image(source, this);
	CFRelease(source);
#else
	corona::Image *tmp = corona::OpenImage(fname, corona::PF_R8G8B8A8);

	if (!tmp) return false; // failed

	copy_image(tmp, this);

	delete tmp;
#endif
	cout << "loading image: " << glfwGetTime() - start << endl;
	return true;
}

void Image::draw(int x, int y) {
	/*
	float coords[] = {
		x, y,
		x+width, y,
		x+width, y+height,
		x, y+height
	};
	*/

	glBindTexture(GL_TEXTURE_2D, texid);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2d(x, y);
		glTexCoord2f(1, 0);
		glVertex2d(x + width, y);
		glTexCoord2f(1, 1);
		glVertex2d(x + width, y + height);
		glTexCoord2f(0, 1);
		glVertex2d(x, y + height);
	glEnd();
}

void Image::blit(Rect src, Rect dest) {
	src.x = src.x / width;
	src.y = src.y / height;

	src.w = src.w / width;
	src.h = src.h / height;

	glBindTexture(GL_TEXTURE_2D, texid);
	glBegin(GL_QUADS);
		glTexCoord2f(src.x, src.y);
		glVertex2d(dest.x, dest.y);
		glTexCoord2f(src.x + src.w, src.y);
		glVertex2d(dest.x + dest.w, dest.y);
		glTexCoord2f(src.x + src.w, src.y + src.h);
		glVertex2d(dest.x + dest.w, dest.y + dest.h);
		glTexCoord2f(src.x, src.y + src.h);
		glVertex2d(dest.x, dest.y + dest.h);
	glEnd();
}

/**
 * Load an image from disk
 * returns image:userdata
 */
int Image::_new_from_file(lua_State *l) {
	const char *fname = luaL_checkstring(l, 1);
	if (!push_image(l)->load(fname))
		return luaL_error(l, "failed to load image: %s", fname);
	return 1;
}

/**
 * Create an image from width, height, and binary rgba byte array
 * returns image:userdata
 */
int Image::_new_from_raw(lua_State *l) {
	int w = luaL_checkint(l, 1);
	int h = luaL_checkint(l, 2);
	const char *bytes = luaL_checkstring(l, 3);

	int len = lua_objlen(l, 3);

	if (len != w*h*4)
		return luaL_error(l, "bytes do not match given dimensions");

	push_image(l)->create(w, h, bytes);

	return 1;
}

/**
 * Create an image from file in memory (as lua string)
 * returns image:userdata
 */
int Image::_new_from_memory(lua_State *l) {
	const char *bytes = luaL_checkstring(l, 1);
	int len = lua_objlen(l, 1);
	if (!push_image(l)->load_memory(bytes, len))
		return luaL_error(l, "failed to load image from memory");
	return 1;
}


int Image::_draw(lua_State *l) {
	Point p = Point::pop(l);
	getself(Image)->draw(p.x, p.y);
	return 0;
}

int Image::_size(lua_State *l) {
	Image *self = getself(Image);
	lua_pushnumber(l, self->width);
	lua_pushnumber(l, self->height);
	return 2;
}

int Image::_blit(lua_State *l) {
	Rect dest = Rect::pop(l);
	Rect src = Rect::pop(l);

	getself(Image)->blit(src, dest);
	return 0;
}

int Image::_bind(lua_State *l) {
	Image *self = getself(Image);
	self->bind();
	return 0;
}

int Image::_raw_update(lua_State *l) {
	Image *self = getself(Image);

	int x = luaL_checkint(l, 2);
	int y = luaL_checkint(l, 3);

	int w = luaL_checkint(l, 4);
	int h = luaL_checkint(l, 5);

	const char *bytes = luaL_checkstring(l, 6);
	int len = lua_objlen(l, 6);

	if (len != w*h*4)
		return luaL_error(l, "bytes do not match given dimensions");

	self->update(x, y, w, h, bytes);
	cout << "updating image: " << len << " bytes" << endl;
	return 0;
}

/**
 * Loads and image from file name, and returns a byte string of pixels
 * expensive due to copy
 *
 * returns width:number, height:number, bytes:string
 */
int Image::_get_image_bytes(lua_State *l) {
	unsigned int width, height;
	Pixel *buffer;

	const char *fname = luaL_checkstring(l, 1);

#ifdef USE_CORONA
	corona::Image *tmp = corona::OpenImage(fname, corona::PF_R8G8B8A8);
	if (!tmp) return luaL_error(l, "failed to open image");
	width = tmp->getWidth();
	height = tmp->getHeight();
	buffer = (Pixel*)tmp->getPixels();

	apply_color_key(width, height, buffer, Pixel(255, 0, 255));

	lua_pushinteger(l, width);
	lua_pushinteger(l, height);
	lua_pushlstring(l, (const char *)buffer, width*height*4);

	delete tmp;
	return 3;
#else
	return 0;
#endif
}

static int next_p2(int x) {
	int size = 1;
	while (size < x) {
		size = size << 1;
	}
	return size;
}

}

