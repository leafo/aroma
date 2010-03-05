
#ifndef IMAGE_H_
#define IMAGE_H_

#include "common.h"
#include "geometry.h"

class Image { 
public: 
	GLuint texid;	
	int width, height;
	int t_width, t_height; // the actual size of texture

	int load(const char *fname); // load image from disk
	void blit(Rect src, Rect dest);

	static void install(lua_State *l); // load the Image metatable
	static Image pop(lua_State *l);
	static void push(lua_State *l, Image *i); // don't use this

	// instance functions
	static int _new(lua_State *l);
	static int _draw(lua_State *l);
	static int _blit(lua_State *l);
};


#endif /* IMAGE_H_ */
