
#include "geometry.h"

#define POOL_SIZE 4

namespace aroma {

	double pool[POOL_SIZE];

	// count - how many things to pull, max of POOL_SIZE
	void pop_tuple(lua_State *l, int count) {
		if (count > POOL_SIZE) count = POOL_SIZE;

		int popc = count;
		if (lua_istable(l, -1)) {
			for (int i = 0; i < count; i++) {
				lua_rawgeti(l, -(i+1), i+1);
			}
			popc++;
		}

		for (int i = 0; i < count; i++) {
			pool[i] = luaL_checknumber(l, -(count - i));
		}

		lua_pop(l, popc);
	}

	// create a new rectangle from a point
	Rect Rect::from_point(Point p, double _w, double _h) {
		Rect r;
		r.x = p.x;
		r.y = p.y;
		r.w = _w;
		r.h = _h;
		return r;
	}

	// pop rectangle from stack
	Rect Rect::pop(lua_State *l) {
		pop_tuple(l, 4);
		Rect r = { pool[0], pool[1], pool[2], pool[3] };
		return r;
	}

	void Point::print() {
		log("Point <%f, %f, %f>\n", x, y, z);
	}


	Point Point::subtract(Point other) {
		Point p = {0};
		p.x = x - other.x;
		p.x = y - other.y;
		p.x = z - other.z;
		return p;
	}


	/**
	 * calculate cross product for two points
	 */
	Point Point::cross(Point other) {
		Point p = {0};
		p.x = (y*other.z-z*other.y);
		p.y = (z*other.x-x*other.z);
		p.z = (x*other.y-y*other.x);
		return p;
	}


	Point Point::operator*(const Mat4 & mat) const {
		Point p = {0};

		p.x = x * mat.data[0] + y * mat.data[4] + z * mat.data[8] + w * mat.data[12];
		p.y = x * mat.data[1] + y * mat.data[5] + z * mat.data[9] + w * mat.data[13];
		p.z = x * mat.data[2] + y * mat.data[6] + z * mat.data[10] + w * mat.data[14];
		p.w = x * mat.data[3] + y * mat.data[7] + z * mat.data[11] + w * mat.data[15];

		return p;
	}

	Point Point::from_array(double *values, int count) {
		Point p;
		switch (count) {
			case 4: p.w = values[3];
			case 3: p.z = values[2];
			case 2: p.y = values[1];
			case 1: p.x = values[0];
		}
		return p;
	}

	// point constructor
	int Point::_new(lua_State *l) {
		push(l, pop(l));
		return 1;
	}

	int Point::_print(lua_State *l) {
		pop(l).print();
		return 0;
	}

	// read either two numbers from table, or 2 integers
	Point Point::pop(lua_State *l) {
		pop_tuple(l, 2);
		Point p = { pool[0], pool[1], 0, 0};

		return p;
	} 

	// pop a 3 dimensional point
	Point Point::pop3(lua_State *l) {
		pop_tuple(l, 3);
		Point p = { pool[0], pool[1], pool[2], 0};

		return p;
	} 


	void Point::push(lua_State *l, double x, double y) {
		lua_newtable(l);	
		lua_pushnumber(l, x);
		lua_rawseti(l, -2, 1);

		lua_pushnumber(l, y);
		lua_rawseti(l, -2, 2);

		luaL_newmetatable(l, "Point");
		lua_setmetatable(l, -2);
	}

	void Point::push(lua_State *l, Point p) {
		return push(l, p.x, p.y);
	}

	Color::Color() : r(255), g(255), b(255), a(255) {};

	Color::Color(byte r, byte g, byte b) 
		: r(r), g(g), b(b), a(255) { }

	Color::Color(byte r, byte g, byte b, byte a)
		: r(r), g(g), b(b), a(a) { }

	void Color::print() {
		log("Color <%d, %d, %d, %d>\n", r,g,b,a);
	}

	Color Color::pop(lua_State *l) {
		// if it is a table there might be an alpha
		if (lua_istable(l, -1) && lua_objlen(l, -1) == 4) {
			pop_tuple(l, 4);
			return Color(pool[0], pool[1], pool[2], pool[3]);
		} 

		pop_tuple(l, 3);
		return Color(pool[0], pool[1], pool[2]);
	}

	void Color::bind() {
#ifndef AROMA_NACL
		glColor4ub(r,g,b,a);
#endif
	}

	byte Color::MAX = 255;
	Color Color::White = Color(255,255,255);
	Color Color::Black = Color(0,0,0);
	Color Color::Red = Color(255,0,0);
	Color Color::Green = Color(0,255,0);
	Color Color::Blue = Color(0,0,255);
	Color Color::Gray = Color(200,200,200);

	float Color::rf() const {
		return 1.0 * r / MAX;
	}

	float Color::gf() const {
		return 1.0 * g / MAX;
	}

	float Color::bf() const {
		return 1.0 * b / MAX;
	}

	float Color::af() const {
		return 1.0 * a / MAX;
	}
}

