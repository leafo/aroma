
#ifndef WINDOW_H_
#define WINDOW_H_

#include "common.h"
#include "geometry.h"

class Window {
public:
	int width, height;

	static Window *instance;
	static Window *create_window(int width, int height, const char *title); 

	static void pushKeytable(lua_State *l);

	// instance functions
	static int _keyDown(lua_State *l);
	static int _hideMouse(lua_State *l);
	static int _showMouse(lua_State *l);
};

#endif /* WINDOW_H_ */

