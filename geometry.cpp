
#include "common.h"

#define POOL_SIZE 4

double pool[POOL_SIZE];

// count - how many things to pull, max of 4
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
	pop_tuple(l, 4);
	Rect r = { pool[0], pool[1], pool[2], pool[3] };
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
	pop_tuple(l, 2);
	Point p = { pool[0], pool[1] };

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


Color Color::pop(lua_State *l)
{
	// if it is a table there might be an alpha
	if (lua_istable(l, -1) && lua_objlen(l, -1) == 4) {
		pop_tuple(l, 4);
		return Color(pool[0], pool[1], pool[2], pool[3]);
	} 

	pop_tuple(l, 3);
	return Color(pool[0], pool[1], pool[2]);
}

void Color::bind() {
	glColor4ub(r,g,b,a);
}

