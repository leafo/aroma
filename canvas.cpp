
#include "common.h"

static Color pop_color(lua_State *l);

void Canvas::push(lua_State *l)
{
	lua_newtable(l);

	// functions
	setfunction("rect", Canvas::_rect);
	setfunction("reshape", Canvas::_reshape);
	setfunction("clearColor", Canvas::_clearColor);
	setfunction("flush", Canvas::_flush);
	setfunction("image", Image::_new);
	setfunction("font", Font::_new);
	setfunction("map", TileMap::_new);

	// properties
	setnumber("dt", 0);
	setnumber("time", glfwGetTime());

	// mouse input
	lua_newtable(l);
	setint("x", 0);
	setint("y", 0);
	setbool("left", false);
	setbool("right", false);

	lua_setfield(l, -2, "mouse");
	
	// create the input table
	lua_newtable(l);
	setnumber("xaxis", 0);
	setnumber("yaxis", 0);

	setbool("left", false);
	setbool("right", false);
	setbool("up", false);
	setbool("down", false);

	setbool("a", false);
	setbool("b", false);
	setbool("c", false);
	setbool("d", false);

	setbool("start", false);
	setbool("select", false);

	setbool("l", false);
	setbool("r", false);

	lua_setfield(l, -2, "input");

	// resize it
	Point::push(l, win->width, win->height);
	Canvas::_reshape(l);
}


// args [ canvas, point ]
int Canvas::_reshape(lua_State *l) 
{
	Canvas *c = win->canvas;

	Point p = Point::pop(l);
	c->width = p.x;
	c->height = p.y;
	
	if (!lua_istable(l, -1)) 
		luaL_error(l, "fatal error: expected canvas table for reshape");

	// update the table
	setint("width", c->width);
	setint("height", c->height);

	int width, height;	
	glfwGetWindowSize(&width, &height);

	glViewport(0,0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, c->width, c->height, 0);

	glMatrixMode(GL_PROJECTION);
	
	return 0;
}

int Canvas::_clearColor(lua_State *l) 
{
	if (lua_gettop(l) == 2) {
		Color c = pop_color(l);
		glClearColor(c.rf(),c.gf(),c.bf(),0);
		return 0;
	} 

	// return the clear color
	return luaL_error(l, "put return clear color here");
}

int Canvas::_flush(lua_State *l) 
{
	if (!lua_istable(l, 1))
		return luaL_error(l, "canvas expected as first argument");

	glfwSwapBuffers();
	glfwSleep(0.005);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the game time
	double time = glfwGetTime();

	lua_getfield(l, 1, "time");
	double old = luaL_checknumber(l, -1);	
	lua_pop(l, 1);

	setnumber("time" , time);
	setnumber("dt" , time - old);

	// find out mouse coords
	int x, y;
	float xx, yy;
	glfwGetMousePos(&x, &y);
	Canvas *c = win->canvas;
	xx = (c->width / win->width) * x;
	yy = (c->height / win->height) * y;

	// update the mouse point
	lua_getfield(l, 1, "mouse");
	setint("x", xx);
	setint("y", yy);

	setbool("left", glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT));
	setbool("right", glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT));


	lua_getfield(l, 1, "input");
	float axis[2];
	glfwGetJoystickPos(GLFW_JOYSTICK_1, axis, 2);

	setnumber("xaxis", axis[0]);
	setnumber("yaxis", axis[1]);

	lua_pushboolean(l, glfwGetWindowParam(GLFW_OPENED) > 0);

	return 1;
}

int Canvas::_rect(lua_State *l)
{
	Point a = Point::pop(l);
	Point b = Point::pop(l);

	// glColor4ub(255,255,255, 255);
	glBegin(GL_QUADS);
		glVertex2d(a.x, a.y);
		glVertex2d(b.x, a.y);
		glVertex2d(b.x, b.y);
		glVertex2d(a.x, b.y);
	glEnd();

	return 0;
}


// pop a color off the head of the table
static Color pop_color(lua_State *l)
{
	if (!lua_istable(l, -1))
		luaL_error(l, "fatal error: failed to find color for argument");

	lua_rawgeti(l,-1, 1);
	lua_rawgeti(l,-2, 2);
	lua_rawgeti(l,-3, 3);

	int r = luaL_checkint(l, -3); 
	int g = luaL_checkint(l, -2); 
	int b = luaL_checkint(l, -1); 

	// pop the table and values
	lua_pop(l, 4);

	return Color(r,g,b);
}


