#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <GL/glfw.h>

using namespace std;


#define setint(name,val) lua_pushinteger(l, val);\
	lua_setfield(l, -2, name)

#define setnumber(name,val) lua_pushnumber(l, val);\
	lua_setfield(l, -2, name)

#define setbool(name,val) lua_pushboolean(l, val);\
	lua_setfield(l, -2, name)

#define setfunction(name,val) lua_pushcfunction(l, val);\
	lua_setfield(l, -2, name)


/**
 * objects
 */

typedef unsigned char byte;
struct Color {
	byte r, g, b, a;

	Color(byte r, byte g, byte b) 
		: r(r), g(g), b(b), a(255) { }

	Color(byte r, byte g, byte b, byte a) 
		: r(r), g(g), b(b), a(a) { }

	Color() { };

	void bind(); // bind color to opengl current

	// return the floating point values
	double rf();
	double gf();
	double bf();

	static Color pop(lua_State *l);
	static void Push(lua_State *l, byte r, byte g, byte b);

	static byte MAX;

	static Color White;
	static Color Black;
	static Color Red;
	static Color Blue;
	static Color Green;
	static Color Gray;
};

struct Point {
	double x, y;

	static void install(lua_State *l);

	static int _new(lua_State *l);
	static int _print(lua_State *l);

	static Point pop(lua_State *l);
	static void push(lua_State *l, double x, double y);
	static void push(lua_State *l, Point p);
};

struct Rect {
	double x, y, w, h;

	static Rect fromPoint(Point p, double _w, double _h);

	static Rect pop(lua_State *l);
};

class Canvas;
class Window {
public:
	Canvas *canvas;
	int width, height;

	static bool created;
	static void install(lua_State *l);

	static void pushKeytable(lua_State *l);

	// instance functions
	static int _new(lua_State *l);
	static int _keyDown(lua_State *l);
	static int _hideMouse(lua_State *l);
	static int _showMouse(lua_State *l);
};

class Canvas {
public:
	Window *w;
	double width, height;
	Color paint;
	Color clearColor;

	static void push(lua_State *l); // create a new canvas

	// instance functions
	static int _clearColor(lua_State *l);
	static int _reshape(lua_State *l);
	static int _rect(lua_State *l);
	static int _flush(lua_State *l);
	static int _setMouse(lua_State *l);
};

class Image { 
public: 
	GLuint texid;	
	int width, height;
	int t_width, t_height; // the actual size of texture

	int load(const char *fname); // load image from disk
	void blit(Rect src, Rect dest);

	static void install(lua_State *l); // load the Image metatable
	static Image pop(lua_State *l);
	static void push(lua_State *l, Image *i); // don't use this

	// instance functions
	static int _new(lua_State *l);
	static int _draw(lua_State *l);
	static int _blit(lua_State *l);
};

class TileSet {
public:
	Image tiles;
	int tw, th; // tile width/height
	int xstride, ystride;

	int load(const char *fname, int w, int h);

	void tile(Point dest, int id); // blit a single tile
};


// creates a bunch of vertex arrays for rendering a 
// map. make sure to delete it when you are done!
class TileMap {
public:
	TileSet tileset;
	int *tiles;
	int width, height; // width/height of map in tiles

	int load(const char *fname, int tw, int th, 
			int *cells, int w, int h);
	void draw(Point viewport);

	static void install(lua_State *l);

	static int _new(lua_State *l);
	static int _draw(lua_State *l);
	static int _width(lua_State *l);
	static int _height(lua_State *l);
};

class Font : public TileSet {
public:
	int realwidth, realheight;

	// void letter(Point dest, char l);
	void string(Point dest, const char *str);

	static void install(lua_State *l);
	static Font pop(lua_State *l);

	static int _new(lua_State *l);
	static int _string(lua_State *l); // draw string
};


extern Window *win; // the window

/* main.cpp */
void stackDump(lua_State *L);

Rect pop_rect(lua_State *l);

template <class T>
T* pushStruct(lua_State *l, const char* meta);

#endif /* COMMON_H_ */

