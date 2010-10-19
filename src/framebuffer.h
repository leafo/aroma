
#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "common.h"
#include "image.h"

class FrameBuffer {
public:
	int width, height;
	Image tex;
	GLuint fbo;
	GLuint depth;

	static int _new(lua_State *l);
	static int _bind(lua_State *l);
	static int _release(lua_State *l);
	static int _render(lua_State *l);

	static int _draw(lua_State *l);
	static int _bindTex(lua_State *l);
	// draw the image
};

#endif