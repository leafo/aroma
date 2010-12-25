
#ifndef WINDOW_H_
#define WINDOW_H_

#include "common.h"
#include "geometry.h"

class Window {
public:
	int width, height;

	static Window *instance;
	static Window *create_window(int width, int height, const char *title); 
};

#endif /* WINDOW_H_ */

