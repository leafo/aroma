
#ifndef CANVAS_H_
#define CANVAS_H_

#include "common.h"
#include "geometry.h"
#include "window.h"
#include "image.h"
#include "font.h"
#include "tiles.h"
#include "mesh.h"
#include "framebuffer.h"

extern Window *win;

struct Viewport {
	void set(double t, double r, double b, double l);
	double top, left, bottom, right;
	bool is2d;
	double fov; // field of view for 3d mode

	Viewport();
	void reshape(); // reshape opengl to this viewport
	double getWidth();
	double getHeight();
};

class Canvas {
public:
	Canvas();
	void push(lua_State *l); // push this canvas to top of stack
	void reshape(); // shape the projection matrix

	Viewport view;

	// double width, height;
	Color paint;
	Color clearColor;

	// static void push(lua_State *l); // create a new canvas

	// instance functions
	static int _clearColor(lua_State *l);
	static int _clear(lua_State *l);

	static int _view2d(lua_State *l);


	static int _view3d(lua_State *l);
	static int _look(lua_State *l);
	static int _strip(lua_State *l);

	static int _rotate(lua_State *l);
	static int _scale(lua_State *l);
	static int _translate(lua_State *l);

	static int _getTime(lua_State *l);

	static int _rect(lua_State *l);
	static int _line(lua_State *l);
	static int _flush(lua_State *l);
	static int _setMouse(lua_State *l);

};

#endif /* CANVAS_H_ */

