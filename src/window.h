
#ifndef WINDOW_H_
#define WINDOW_H_

#include "common.h"
#include "geometry.h"

class Canvas;
class Canvas2d;
class Canvas3d;

class Window {
public:
	Canvas *canvas;
	int width, height;

	static bool created;
	static void install(lua_State *l);

	static void pushKeytable(lua_State *l);

	// instance functions
	static int _new(lua_State *l);
	static int _keyDown(lua_State *l);
	static int _hideMouse(lua_State *l);
	static int _showMouse(lua_State *l);
};

#include "canvas.h"

#endif /* WINDOW_H_ */

