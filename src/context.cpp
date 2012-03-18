
#include "context.h"
#include "common.h"

namespace aroma {
#ifndef AROMA_NACL

	GLFWContext::GLFWContext(int w, int h, const char* title)
		: w(w), h(h), title(title), created(false) { }

	bool GLFWContext::make_window() {
		glfwInit();	

#ifdef __APPLE__
		chdir(aroma_current_dir);
#endif
		glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
		bool ok = glfwOpenWindow(
				w, h,
				8, 8, 8,
				8, // alpha buffer
				24, // depth buffer
				0, // stencil buffer
				GLFW_WINDOW
				);

		if (!ok) {
			glfwTerminate();
			return false;
		}

		glfwSetWindowTitle(title);
		glfwEnable(GLFW_STICKY_KEYS);

		// glfwSetWindowSizeCallback(resize);

		/* */
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/* */

		glShadeModel(GL_FLAT);
		// glEnable(GL_LIGHTING);
		// glEnable(GL_LIGHT0);

		GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
		GLfloat diffuse[] = {0.9, 0.9, 0.9, 1.0};
		GLfloat specular[] = {0.5, 0.5, 0.5, 1.0};

		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);	


		// glEnable(GL_COLOR_MATERIAL);
		// glColorMaterial(GL_FRONT, GL_DIFFUSE);
		created = true;
		return true;
	}

	bool GLFWContext::make_current() {
		if (!created && !make_window()) return false;
		return true;
	}

	void GLFWContext::resize(const int w, const int h) {
		log("glfw window can't resize!");
	}

	void GLFWContext::flush() {
		glfwSwapBuffers();
		glfwSleep(0.005);
	}

	int GLFWContext::width() {
		return w;
	}

	int GLFWContext::height() {
		return h;
	}

	double GLFWContext::get_time() {
		return glfwGetTime();
	}

#endif

}
