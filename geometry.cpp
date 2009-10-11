
#include "common.h"


// create a new rectangle from a point
Rect Rect::fromPoint(Point p, double _w, double _h)
{
	Rect r;
	r.x = p.x;
	r.y = p.y;
	r.w = _w;
	r.h = _h;
	return r;
}

// pop rectangle from stack
Rect Rect::pop(lua_State *l)
{
	Rect r;
	
	int count = 4;
	if (lua_istable(l, -1)) {
		count = 5;
		lua_rawgeti(l, -1, 1);
		lua_rawgeti(l, -2, 2);
		lua_rawgeti(l, -3, 3);
		lua_rawgeti(l, -4, 4);
	}

	r.x = luaL_checknumber(l, -4);
	r.y = luaL_checknumber(l, -3);
	r.w = luaL_checknumber(l, -2);
	r.h = luaL_checknumber(l, -1);

	lua_pop(l, count);

	return r;
}


// install point metatable and constructor
void Point::install(lua_State *l)
{
	luaL_newmetatable(l, "Point");

	lua_newtable(l); // create the index table
	setfunction("print", _print);

	lua_setfield(l, -2, "__index"); // set index in metatable

	// now register the function on global
	lua_pushcfunction(l, _new);
	lua_setfield(l, LUA_GLOBALSINDEX, "Point");
}

// point constructor
int Point::_new(lua_State *l) 
{
	push(l, pop(l));
}

int Point::_print(lua_State *l)
{
	Point p = pop(l);
	cout << "Point: " << p.x << ", " << p.y << endl;
	return 0;
}

// read either two numbers from table, or 2 integers
Point Point::pop(lua_State *l) 
{
	Point p;
	
	int count = 2; // pop count
	if (lua_istable(l, -1)) {
		count = 3;
		lua_rawgeti(l, -1, 1);
		lua_rawgeti(l, -2, 2);
	} 

	p.x = luaL_checknumber(l, -2);
	p.y = luaL_checknumber(l, -1);

	lua_pop(l, count);
	return p;
}

void Point::push(lua_State *l, double x, double y)
{
	lua_newtable(l);	
	lua_pushnumber(l, x);
	lua_rawseti(l, -2, 1);

	lua_pushnumber(l, y);
	lua_rawseti(l, -2, 2);

	luaL_newmetatable(l, "Point");
	lua_setmetatable(l, -2);
}

void Point::push(lua_State *l, Point p)
{
	return push(l, p.x, p.y);
}


