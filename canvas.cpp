
#include "canvas.h"

Viewport::Viewport() {
	is2d = true;
	set(0, win->width, win->height, 0);
}

void Viewport::set(double t, double r, double b, double l) {
	top = t;
	right = r;
	bottom = b;
	left = l;
}

double Viewport::getWidth() {
	return right - left;
}

double Viewport::getHeight() {
	return bottom - top;
}

void Viewport::reshape() {
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();
	if (is2d) {
		// cout << $(top) << $(right) << $(bottom) << $(left) << endl;
		gluOrtho2D(left, right, bottom, top);
	} else {
		gluPerspective(fov, (double)getWidth()/getHeight(), 1, 50);
	}
	glMatrixMode(GL_MODELVIEW);
}


Canvas::Canvas() {
	// TODO
	// reshape();
}

void Canvas::push(lua_State *l) {
	lua_newtable(l);

	// functions
	setfunction("rect", Canvas::_rect);
	setfunction("line", Canvas::_line);

	setfunction("view2d", Canvas::_view2d);
	setfunction("view3d", Canvas::_view3d);
	setfunction("look", Canvas::_look);
	setfunction("strip", Canvas::_strip);

	setfunction("rotate", Canvas::_rotate);
	setfunction("scale", Canvas::_scale);
	setfunction("translate", Canvas::_translate);

	setfunction("getTime", Canvas::_getTime);
	setfunction("clearColor", Canvas::_clearColor);
	setfunction("clear", Canvas::_clear);
	setfunction("flush", Canvas::_flush);
	setfunction("setMouse", Canvas::_setMouse);

	setfunction("image", Image::_new);
	setfunction("font", Font::_new);
	setfunction("map", TileMap::_new);
	setfunction("mesh", Mesh::_new);

	setfunction("framebuffer", FrameBuffer::_new);

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

	setnumber("width", view.right - view.left);
	setnumber("height", view.bottom - view.top);

	lua_setfield(l, -2, "input");
}

int Canvas::_getTime(lua_State *l) {
	lua_pushnumber(l, glfwGetTime());
	return 1;
}

// set the viewport directly
int Canvas::_view2d(lua_State *l) {
	Canvas *c = win->canvas;
	Point br = Point::pop(l);
	Point tl = Point::pop(l);

	// if we have the canvas table, update it
	if (lua_istable(l, -1)) {
		setnumber("width", c->view.right - c->view.left);
		setnumber("height", c->view.bottom - c->view.top);
	}

	glDisable(GL_DEPTH_TEST);

	c->view.is2d = true;
	c->view.set(tl.y, br.x, br.y, tl.x);
	c->view.reshape();
	return 0;
}

int Canvas::_view3d(lua_State *l) {
	Canvas *c = win->canvas;
	double fov = luaL_checknumber(l, -1);

	glEnable(GL_DEPTH_TEST);

	c->view.is2d = false;
	c->view.fov = fov;
	c->view.reshape();
	return 0;
}

int Canvas::_look(lua_State *l) {
	Canvas *c = win->canvas;
	if (c->view.is2d) return 0;
	Point center = Point::pop3(l);
	Point eye = Point::pop3(l);

	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, 0,1,0);

	// set the light position
	GLfloat lightPos[] = {2,2,2, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glPointSize(10.0);
	glBegin(GL_POINTS);
	glVertex3f(2,2,2);
	glEnd();

	return 0;
}

int Canvas::_strip(lua_State *l) {
	// draw a cube or something
	static double vert[] = {
		-1, 1, 0,
		1, 1, 0,
		1, -1, 0,
		-1, -1, 0,
	};

	glVertexPointer(3, GL_DOUBLE, 0, vert);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_VERTEX_ARRAY);

	return 0;
}


int Canvas::_clearColor(lua_State *l) 
{
	/*
	if (lua_gettop(l) == 1) {
		// return the clear color
		return luaL_error(l, "this should return clear color");
	} 
	*/

	Color c = Color::pop(l);
	glClearColor(c.rf(),c.gf(),c.bf(),1);
	return 0;
}

int Canvas::_clear(lua_State *l) {
	Canvas::_clearColor(l);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return 0;
}

int Canvas::_flush(lua_State *l) 
{
	if (!lua_istable(l, 1))
		return luaL_error(l, "canvas expected as first argument");

	Canvas *c = win->canvas;

	glfwSwapBuffers();
	glfwSleep(0.005);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// if (!c->is2d)
	glLoadIdentity();

	// set the game time
	double time = glfwGetTime();

	lua_getfield(l, 1, "time");
	double old = luaL_checknumber(l, -1);	
	lua_pop(l, 1);

	setnumber("time" , time);
	setnumber("dt" , time - old);

	int x, y;
	double cx, cy; // canvas mouse coordinates
	glfwGetMousePos(&x, &y);

	if (c->view.is2d) {
		Viewport & view = c->view;
		double vwidth = view.right - view.left;
		double vheight = view.bottom - view.top;

		cx = view.left + (x * vwidth / win->width);
		cy = view.top + (y * vheight / win->height);
	} else {
		cx = (double)x;
		cy = (double)y;
	}

	// update the mouse point
	lua_getfield(l, 1, "mouse");
	setnumber("x", cx);
	setnumber("y", cy);

	setbool("left", glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT));
	setbool("right", glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT));


	lua_getfield(l, 1, "input");
	float axis[2];
	glfwGetJoystickPos(GLFW_JOYSTICK_1, axis, 2);

	setnumber("xaxis", axis[0]);
	setnumber("yaxis", axis[1]);


	setbool("left", glfwGetKey(GLFW_KEY_LEFT));
	setbool("right", glfwGetKey(GLFW_KEY_RIGHT));
	setbool("up", glfwGetKey(GLFW_KEY_UP));
	setbool("down", glfwGetKey(GLFW_KEY_DOWN));

	setbool("a", glfwGetKey('Q'));
	setbool("b", glfwGetKey('W'));

	lua_pushboolean(l, glfwGetWindowParam(GLFW_OPENED) > 0);

	return 1;
}

/**
 * Set the current mouse location
 */
int Canvas::_setMouse(lua_State *l) {
	Canvas *c = win->canvas;
	Point p = Point::pop(l);

	if (c->view.is2d) {
		Viewport & view = win->canvas->view;
		double vwidth = view.right - view.left;
		double vheight = view.bottom - view.top;

		// convert from canvas to window coordinates
		double wx = (p.x - view.left) * (win->width / vwidth);
		double wy = (p.y - view.top) * (win->height / vheight);
		glfwSetMousePos((int)wx, (int)wy);
	} else {
		glfwSetMousePos((int)p.x, (int)p.y);
	}

	return 0;
}

int Canvas::_line(lua_State *l) {
	Color c = Color::pop(l);
	Point a = Point::pop(l);
	Point b = Point::pop(l);

	glDisable(GL_TEXTURE_2D);
	c.bind();
	glBegin(GL_LINES);
		glVertex2d(a.x, a.y);
		glVertex2d(b.x, b.y);
	glEnd();
	Color::White.bind();
	glEnable(GL_TEXTURE_2D);

	return 0;
}


int Canvas::_rect(lua_State *l)
{
	Color c = Color::pop(l);
	Point a = Point::pop(l);
	Point b = Point::pop(l);

	glDisable(GL_TEXTURE_2D);
	c.bind();
	glBegin(GL_QUADS);

		glTexCoord2d(0,0);
		glVertex2d(a.x, a.y);

		glTexCoord2d(1,0);
		glVertex2d(b.x, a.y);

		glTexCoord2d(1,1);
		glVertex2d(b.x, b.y);

		glTexCoord2d(0,1);
		glVertex2d(a.x, b.y);
	glEnd();
	Color::White.bind();
	glEnable(GL_TEXTURE_2D);

	return 0;
}


int Canvas::_rotate(lua_State *l) {
	double axis_z = luaL_checknumber(l, -1);
	double axis_y = luaL_checknumber(l, -2);
	double axis_x = luaL_checknumber(l, -3);

	double angle = luaL_checknumber(l, -4);

	glRotated(angle, axis_x, axis_y, axis_z);
	return 0;
}

int Canvas::_scale(lua_State *l) {
	double s = luaL_checknumber(l, -1);
	glScaled(s,s,s);
	return 0;
}


int Canvas::_translate(lua_State *l) {
	Point p = Point::pop(l);

	glTranslated(p.x, p.y, p.z);

	return 0;
}





