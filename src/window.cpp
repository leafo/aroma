#include "window.h"

#include "shader.h"

Window *win = 0;

bool Window::created = false;

static void resize(int x, int y) {
	glViewport(0,0, x, y);
}

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
	lua_pushinteger(l, GLFW_KEY_LSHIFT);
	lua_setfield(l, -2, "shift");
	lua_pushinteger(l, GLFW_KEY_ENTER);
	lua_setfield(l, -2, "enter");
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

	glfwSetWindowSizeCallback(resize);

	/* */
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/* */

	glShadeModel(GL_FLAT);
	/*
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	*/
	
	GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat diffuse[] = {0.9, 0.9, 0.9, 1.0};
	GLfloat specular[] = {0.5, 0.5, 0.5, 1.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);	


	// glEnable(GL_COLOR_MATERIAL);
	// glColorMaterial(GL_FRONT, GL_DIFFUSE);


	// create the window table
	lua_newtable(l);
	setint("width", width);
	setint("height", height);
	setbool("running", true);

	setfunction("keyDown", Window::_keyDown);
	setfunction("showMouse", Window::_showMouse);
	setfunction("hideMouse", Window::_hideMouse);

	setfunction("shader", Shader::_new);

	Window::pushKeytable(l);
	lua_setfield(l, -2, "key");

	win->canvas = new Canvas();
	win->canvas->push(l);

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


int Window::_hideMouse(lua_State *l) {
	glfwDisable(GLFW_MOUSE_CURSOR);
	return 0;
}

int Window::_showMouse(lua_State *l) {
	glfwEnable(GLFW_MOUSE_CURSOR);	
	return 0;
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


