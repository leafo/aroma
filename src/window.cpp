#include "window.h"

#include "shader.h"

namespace aroma {

Window *Window::instance = NULL;

static void resize(int x, int y) {
	glViewport(0,0, x, y);
}

Window *Window::create_window(int width, int height, const char *title) {
	if (Window::instance == 0) {
		glfwInit();	

#ifdef __APPLE__
		chdir(aroma_current_dir);
#endif

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
			return NULL;
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

		Window *win = Window::instance = new Window;
		win->width = width;
		win->height = height;
	}

	return Window::instance;
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

}


