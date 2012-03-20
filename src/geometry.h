
#pragma once
#include "common.h"

namespace aroma {

	struct Color {
		byte r, g, b, a;

		Color();
		Color(byte r, byte g, byte b);
		Color(byte r, byte g, byte b, byte a);

		void print();

		void bind(); // bind color to opengl current

		// return the floating point values
		float rf() const;
		float gf() const;
		float bf() const;
		float af() const;

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
		double x, y, z, w;

		Point cross(Point other);
		Point dot(Point other);
		Point subtract(Point other);
		Point add(Point other);

		void print();

		static Point from_array(double *values, int count);

		static int _new(lua_State *l);
		static int _print(lua_State *l);

		static Point pop(lua_State *l);
		static Point pop3(lua_State *l);

		static void push(lua_State *l, double x, double y);
		static void push(lua_State *l, Point p);
	};

	struct Rect {
		double x, y, w, h;

		static Rect from_point(Point p, double _w, double _h);
		static Rect pop(lua_State *l);
	};

}

