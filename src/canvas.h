
#ifndef CANVAS_H_
#define CANVAS_H_

#include "common.h"
#include "geometry.h"
#include "window.h"

namespace aroma {

struct Viewport {
	void set(double t, double r, double b, double l);
	double top, left, bottom, right;
	bool is2d;
	double fov; // field of view for 3d mode

	Viewport(double width, double height);
	void reshape(); // reshape opengl to this viewport
	double getWidth();
	double getHeight();

	void print();
};

class Canvas {
public:
	Canvas(Window &window);
	void push(lua_State *l); // push this canvas to top of stack
	void reshape(); // shape the projection matrix

	Window &window;
	Viewport view;

	// double width, height;
	Color paint;
	Color clearColor;

	static int _new(lua_State *l); // create new canvas

	static int _call(lua_State *l); // call the canvas object

	// instance methods
	
	static int _run(lua_State *l); // run the game loop

	static int _clearColor(lua_State *l);
	static int _clear(lua_State *l);

	static int _viewport(lua_State *l); // set 2d viewport
	static int _view3d(lua_State *l);

	static int _look(lua_State *l);
	static int _strip(lua_State *l);

	static int _save(lua_State *l);
	static int _restore(lua_State *l);

	static int _rotate(lua_State *l);
	static int _scale(lua_State *l);
	static int _translate(lua_State *l);

	static int _noise(lua_State *l);

	static int _getTime(lua_State *l);

	static int _key(lua_State *l);

	static int _key_down(lua_State *l);
	static int _key_up(lua_State *l);

	static int _rect(lua_State *l);
	static int _line(lua_State *l);
	static int _flush(lua_State *l);

	static int _setMouse(lua_State *l);

	static int _hideMouse(lua_State *l);
	static int _showMouse(lua_State *l);
};

}

#endif /* CANVAS_H_ */

