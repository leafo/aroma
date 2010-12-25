
#include "canvas.h"

#include "set"

using namespace std;

static Canvas *_canvas = NULL; // find a better place to put this

static const char *keys[] = {"esc", "space", "shift", "enter", 0};
static const int key_to_id[] = {
	GLFW_KEY_ESC, GLFW_KEY_SPACE,
	GLFW_KEY_LSHIFT, GLFW_KEY_ENTER
};

static set<int> keys_down;
static set<int> keys_up;

static int push_key_table(lua_State *l);
static int pop_key(lua_State *l);

void GLFWCALL key_listener(int key_id, int action) {
	if (action == GLFW_PRESS) {
		// cout << "pushing key " << key_id << endl;
		keys_down.insert(key_id);
	} else if (action == GLFW_RELEASE) {
		// cout << "releasing key " << key_id << endl;
		keys_up.insert(key_id);
	}
}

Viewport::Viewport(double width, double height) {
	is2d = true;
	set(0, width, height, 0);
}

void Viewport::print() {
	cout << $(top) << $(left) << $(right) << $(bottom) << endl;
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
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glDisable(GL_DEPTH_TEST);
		gluOrtho2D(left, right, bottom, top);
	} else {
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		gluPerspective(fov, (double)getWidth()/getHeight(), 1, 50);
	}
	glMatrixMode(GL_MODELVIEW);
}


Canvas::Canvas(Window &window)
	: window(window), view(window.width, window.height)
{ 
	view.reshape();
	glfwSetKeyCallback(key_listener);
}

/**
 * create a new canvas
 * args: width, height, [title]
 */
int Canvas::_new(lua_State *l) {
	const char *title = "Aroma";

	int width = luaL_checkint(l, 1);
	int height = luaL_checkint(l, 2);
	if (lua_gettop(l) > 2) {
		title = luaL_checkstring(l, 3);
	}

	Window *win = Window::create_window(width, height, title);
	if (!win) return luaL_error(l, "fatal error: failed to open window");

	_canvas = new Canvas(*win);

	Viewport &view = _canvas->view;

	lua_newtable(l);

	// functions
	
	setfunction("run", Canvas::_run);

	setfunction("rect", Canvas::_rect);
	setfunction("line", Canvas::_line);

	setfunction("viewport", Canvas::_viewport);
	setfunction("view3d", Canvas::_view3d);

	setfunction("look", Canvas::_look);
	setfunction("strip", Canvas::_strip);

	setfunction("rotate", Canvas::_rotate);
	setfunction("scale", Canvas::_scale);
	setfunction("translate", Canvas::_translate);

	setfunction("save", Canvas::_save);
	setfunction("restore", Canvas::_restore);

	// setfunction("noise", Canvas::_noise);

	setfunction("getTime", Canvas::_getTime);
	setfunction("clear_color", Canvas::_clearColor);
	setfunction("clear", Canvas::_clear);
	setfunction("flush", Canvas::_flush);

	setfunction("set_mouse", Canvas::_setMouse);
	setfunction("hide_mouse", Canvas::_hideMouse);
	setfunction("show_mouse", Canvas::_showMouse);

	setfunction("key", Canvas::_key);
	setfunction("key_up", Canvas::_key_up);
	setfunction("key_down", Canvas::_key_down);

	setfunction("image", Image::_new);
	setfunction("font", Font::_new);
	setfunction("map", TileMap::_new);
	setfunction("mesh", Mesh::_new);
	setfunction("shader", Shader::_new);

	setfunction("framebuffer", FrameBuffer::_new);

	// properties
	setnumber("dt", 0);
	setnumber("time", glfwGetTime());

	setnumber("width", view.getWidth());
	setnumber("height", view.getHeight());


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

	// the keys
	push_key_table(l);
	lua_setfield(l, -2, "keys");

	// create meta table
	lua_newtable(l);		
	setfunction("__call", _call);
	lua_setmetatable(l, -2);

	return 1;
}

// args: canvas, settings:table
// uses every string key as a function name in the canvas object
// to be called with the value as the only argument
int Canvas::_call(lua_State *l) {
	luaL_checktype(l, 2, LUA_TTABLE);
	lua_pushnil(l);

	bool run_when_done = false;

	while (lua_next(l, 2) != 0) {
		if (lua_type(l, -2) == LUA_TSTRING) {
			lua_pushvalue(l, -2); // push the key on top
			lua_gettable(l, 1); // get value from canvas
			if (lua_isfunction(l, -1)) {
				lua_pushvalue(l, 1); // push canvas table
				lua_pushvalue(l, -3); // push value on top
				lua_call(l, 2, 0);
			} else {
				lua_pop(l, 1); // remove canvas value
				
				lua_pushvalue(l, -2); // copy key
				
				if (lua_isstring(l, -1)) {
					const char *key_name = lua_tostring(l, -1);
					if (strcmp(key_name, "ondraw") == 0) {
						run_when_done = true;
					}
				}

				lua_pushvalue(l, -2); // copy value
				lua_settable(l, 1);
			}
		}
		lua_pop(l, 1); // remove value
	}

	lua_settop(l, 1); // leave behind canvas

	if (run_when_done) {
		lua_pushstring(l, "run");
		lua_gettable(l, 1);
		lua_pushvalue(l, 1);
		lua_call(l, 1, 0);
	}

	return 1;
}

// run the game loop
// args: canvas
// needs ondraw method in canvas table in order to run
int Canvas::_run(lua_State *l) {
	luaL_checktype(l, 1, LUA_TTABLE);

	bool finished = false;
	while (!finished) {
		lua_pushstring(l, "ondraw");
		lua_gettable(l, 1);

		if (!lua_isfunction(l, -1)) {
			return luaL_error(l, "ondraw function is not set");
		}

		lua_pushvalue(l, 1); // canvas
		lua_call(l, 1, 1);

		finished = lua_toboolean(l, -1);

		lua_pop(l, 1); // pop return value

		lua_pushstring(l, "flush");
		lua_gettable(l, 1);
		lua_pushvalue(l, 1);
		lua_call(l, 1, 1);

		finished = finished || !lua_toboolean(l, -1) || glfwGetKey(GLFW_KEY_ESC);
		lua_pop(l, 1); // pop return value
	}

	return 0;
}


// set the 2d viewport
// args: canvas, scale:number | dimensions:table
int Canvas::_viewport(lua_State *l) {
	Viewport view(0,0);
	if (lua_isnumber(l, -1)) {
		double scale = lua_tonumber(l, -1);
		view.right = _canvas->window.width / scale;
		view.bottom = _canvas->window.width / scale;
	} else if (lua_istable(l, -1)) {
		int i = 1;
		switch (lua_objlen(l, -1)) {
			case 4:
				lua_rawgeti(l, -1, i++);
				lua_rawgeti(l, -2, i++);
				view.left = lua_tonumber(l, -2);
				view.top = lua_tonumber(l, -1);
				lua_pop(l, 2);
			case 2:
				lua_rawgeti(l, -1, i++);
				lua_rawgeti(l, -2, i++);
				view.right = lua_tonumber(l, -2);
				view.bottom = lua_tonumber(l, -1);
				lua_pop(l, 2);
				break;
			default:
				return luaL_error(l, "unknown 2d viewport size");
		}
	}

	view.reshape();
	_canvas->view = view;

	// update the canvas
	lua_pushvalue(l, 1);
	setnumber("width", view.getWidth());
	setnumber("height", view.getHeight());

	return 0;
}

// enabled 3d view, set field of view
// args: [canvas], fov
int Canvas::_view3d(lua_State *l) {
	double fov = luaL_checknumber(l, -1);

	Viewport &view = _canvas->view;

	view.is2d = false;
	view.fov = fov;
	view.reshape();
	return 0;
}



// check if a key is pressed
// args: canvas, key:string|number
int Canvas::_key(lua_State *l) {
	int key = pop_key(l);
	lua_pushboolean(l, glfwGetKey(key));
	return 1;
}

int Canvas::_key_down(lua_State *l) {
	int key = pop_key(l);
	lua_pushboolean(l, keys_down.find(key) != keys_down.end());
	return 1;
}

int Canvas::_key_up(lua_State *l) {
	int key = pop_key(l);
	lua_pushboolean(l, keys_up.find(key) != keys_up.end());
	return 1;
}

int Canvas::_getTime(lua_State *l) {
	lua_pushnumber(l, glfwGetTime());
	return 1;
}

// set camera for 3d view
// args: eye:point3 center:point3
int Canvas::_look(lua_State *l) {
	Viewport &view = _canvas->view;
	if (view.is2d) return 0;
	Point center = Point::pop3(l);
	Point eye = Point::pop3(l);

	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, 0,0,1);

	// set the light position
	// TODO: clearly this should not be here
	GLfloat lightPos[] = {2,2,2, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	return 0;
}

// debug function
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

int Canvas::_clearColor(lua_State *l) {
	Color c = Color::pop(l);
	glClearColor(c.rf(),c.gf(),c.bf(),1);
	return 0;
}

int Canvas::_clear(lua_State *l) {
	Canvas::_clearColor(l);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return 0;
}

int Canvas::_flush(lua_State *l) {
	if (!lua_istable(l, 1))
		return luaL_error(l, "canvas expected as first argument");

	// clear key events
	keys_down.clear();
	keys_up.clear();

	glfwSwapBuffers();
	glfwSleep(0.005);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// if (!_canvas->view.is2d)
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

	Window &win = _canvas->window;
	Viewport &view = _canvas->view;
	if (view.is2d) {
		double vwidth = view.right - view.left;
		double vheight = view.bottom - view.top;

		cx = view.left + (x * vwidth / win.width);
		cy = view.top + (y * vheight / win.height);
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
 * Set the mouse location
 */
int Canvas::_setMouse(lua_State *l) {
	Point p = Point::pop(l);

	Window &win = _canvas->window;
	Viewport &view = _canvas->view;
	if (view.is2d) {
		double vwidth = view.right - view.left;
		double vheight = view.bottom - view.top;

		// convert from canvas to window coordinates
		double wx = (p.x - view.left) * (win.width / vwidth);
		double wy = (p.y - view.top) * (win.height / vheight);
		glfwSetMousePos((int)wx, (int)wy);
	} else {
		glfwSetMousePos((int)p.x, (int)p.y);
	}

	return 0;
}

int Canvas::_hideMouse(lua_State *l) {
	glfwDisable(GLFW_MOUSE_CURSOR);
	return 0;
}

int Canvas::_showMouse(lua_State *l) {
	glfwEnable(GLFW_MOUSE_CURSOR);	
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


int Canvas::_rect(lua_State *l) {
	Color c = Color::pop(l);
	Point a = Point::pop(l);
	Point b = Point::pop(l);

	// glPopAttrib (current_bit?)
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

/*
int Canvas::_noise(lua_State *l) {
	int count = lua_gettop(l);
	if (count > 3) count = 3;

	double noise = 0;
	switch (count) {
		case 1:
			noise = PerlinNoise1D(luaL_checknumber(l, 1), 2, 2, 1);
			break;
		case 2:
			noise = PerlinNoise2D(luaL_checknumber(l, 1),
					luaL_checknumber(l, 2), 2, 2, 1);
			break;
		case 3: 
			noise = PerlinNoise3D(luaL_checknumber(l, 1),
					luaL_checknumber(l, 2), luaL_checknumber(l, 3),
					2, 2, 1);
			break;
	}

	lua_pushnumber(l, noise);
	return 1;
}
*/

int Canvas::_save(lua_State *l) {
	return 0;
}

int Canvas::_restore(lua_State *l) {
	return 0;
}

// insert keys from key table into table on top of stack
int push_key_table(lua_State *l) {
	lua_newtable(l);
	const char **key = keys;
	int i = 0;
	while (*key) {
		lua_pushinteger(l, key_to_id[i++]);
		lua_setfield(l, -2, *(key++));
	}

	return 1;
}

// get a key off the top of stack, leaves it how it was
// args: canvas, key:string|number
int pop_key(lua_State *l) {
	int key;

	if (lua_isstring(l, -1)) {
		// check if it is in the key table
		lua_getfield(l, -2, "keys");
		lua_pushvalue(l, -2); // string on top
		lua_gettable(l, -2);
		
		if (lua_isnil(l, -1)) {
			// get the first letter and use it as char
			size_t len;
			const char *key_str = lua_tolstring(l, -3, &len);
			if (len == 0) return luaL_error(l, "invalid key string");
			key = toupper(key_str[0]);
		} else {
			// use value from keys table
			key = lua_tointeger(l, -1); 
		}

		lua_pop(l, 2); // pop key table and value
	} else  {
		key = luaL_checkint(l, -1);
	}

	return key;
}

