
#ifndef IMAGE_H_
#define IMAGE_H_

#include "common.h"
#include "geometry.h"

class Image { 
public: 
	GLuint texid;	
	int width, height;

	int load(const char *fname); // load image from disk
	int create(int width, int height, const void *bytes); 

	void blit(Rect src, Rect dest);
	void draw(int x, int y);
	void bind() const;

	static void install(lua_State *l); // load the Image metatable
	static Image pop(lua_State *l); // don't use this anymore

	// instance functions
	static int _new(lua_State *l);
	static int _draw(lua_State *l);
	static int _blit(lua_State *l);
	static int _bind(lua_State *l);
	static int _size(lua_State *l);
};


#endif /* IMAGE_H_ */
