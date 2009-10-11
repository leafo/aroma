#include "common.h"

Window *win = 0;

bool Window::created = false;

void Window::install(lua_State *l)
{
	lua_pushcfunction(l, Window::_new);
	lua_setglobal(l, "Window"); 
}

void Window::pushKeytable(lua_State *l)
{
	lua_newtable(l);
	lua_pushinteger(l, GLFW_KEY_ESC);
	lua_setfield(l, -2, "esc");
	lua_pushinteger(l, GLFW_KEY_SPACE);
	lua_setfield(l, -2, "space");
}


int Window::_new(lua_State *l)
{
	if (Window::created) {
		return luaL_error(l, "can't make two windows");
	}
	Window::created = true;

	const char *title = luaL_checkstring(l, 1);
	int width = luaL_checkint(l, 2);
	int height = luaL_checkint(l, 3);

	// create opengl
	glfwInit();	
	
	// win = new Window;
	// win = (Window *)lua_newuserdata(l, sizeof(Window));
	// win = pushStruct<Window>(l, "Window");
	win = new Window;
	win->width = width;
	win->height = height;
	
	bool ok = glfwOpenWindow(
		width, height,
		8, 8, 8,
		8, // alpha buffer
		24, // depth buffer
		0, // stencil buffer
		GLFW_WINDOW
	);

	if (!ok) {
		glfwTerminate();
		return luaL_error(l, "fatal error: failed to open window");
	}

	glfwSetWindowTitle(title);
	glfwEnable(GLFW_STICKY_KEYS);

	/* */
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/* */

	// create the window table
	lua_newtable(l);
	setint("width", width);
	setint("height", height);
	setbool("running", true);

	setfunction("keyDown", Window::_keyDown);

	Window::pushKeytable(l);
	lua_setfield(l, -2, "key");

	Canvas *c = new Canvas;
	c->width = width;
	c->height = height;
	win->canvas = c;

	Canvas::push(l);
	lua_setfield(l, -2, "canvas");

	// get canvas back
	lua_getfield(l, -1, "canvas");

	return 2; 
}

/**
 * instance functions
 */


int Window::_keyDown(lua_State *l)
{
	int key = luaL_checkint(l, 1);
	lua_pushboolean(l, glfwGetKey(key));
	return 1;
}


// this is just here for reference, not actually used
// allocate userdata for new struct and push it on
// the stack. Return the pointer to struct
template <class T>
T* pushStruct(lua_State *l, const char* meta)
{
	T* data = (T*)lua_newuserdata(l, sizeof(T));
	luaL_getmetatable(l, meta);
	lua_setmetatable(l, -2);
	return data;	
}


