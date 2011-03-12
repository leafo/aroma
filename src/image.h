
#ifndef IMAGE_H_
#define IMAGE_H_

#include "common.h"
#include "geometry.h"

class Image {
public:
	GLuint texid;
	int width, height;

	bool load(const char *fname); // load image from disk
	bool load_memory(const void *bytes, int size); // load image from memory
	void create(int width, int height, const void *bytes);

	// update a portion of the image texture from bytes
	void update(int x, int y, int width, int height, const void *bytes);

	void blit(Rect src, Rect dest);
	void draw(int x, int y);
	void bind() const;

	// instance functions
	static int _new_from_file(lua_State *l);
	static int _new_from_raw(lua_State *l);
	static int _new_from_memory(lua_State *l);

	static int _get_image_bytes(lua_State *l); // get raw bytes for file name

	static int _draw(lua_State *l);
	static int _blit(lua_State *l);
	static int _bind(lua_State *l);
	static int _size(lua_State *l);

	static int _raw_update(lua_State *l);
};


#endif /* IMAGE_H_ */
