
#include "tiles.h"


int TileSet::load(const char *fname, int w, int h)
{
	if (!tiles.load(fname)) return false;

	tw = w;
	th = h;
	xstride = tiles.width / tw;
	ystride = tiles.height / th;

	return true;
}

// draw a tile to dest
void TileSet::tile(Point dest, int id)
{
	Rect src;
	src.x = tw * (id % xstride);
	src.y = th * (id / ystride);
	src.w = tw;
	src.h = th;

	tiles.blit(src, Rect::fromPoint(dest, tw, th));
}


int TileMap::load(const char *fname, int tw, int th, int *cells, int w, int h)
{
	if (!tileset.load(fname, tw, th)) return false;
	width = w;
	height = h;
	
	tiles = cells;

	return true;
}

// draw the map
void TileMap::draw(Point viewport) 
{
	for (int hh = 0; hh < height; hh++) {
		for (int ww = 0; ww < width; ww++) {
			// cout << tiles[ww + hh * width] << ", ";

			Point p;
			p.x = ww*tileset.tw - viewport.x;
			p.y = hh*tileset.th - viewport.y; 
			tileset.tile(p, tiles[ww + hh * width]);
		}
	}
}


void TileMap::install(lua_State *l)
{
	luaL_newmetatable(l, "Map");
	lua_newtable(l);
	setfunction("draw", TileMap::_draw);

	lua_setfield(l, -2, "__index");
}

int TileMap::_new(lua_State *l)
{
	// args:
	// the tileset
	// point with tile height/width
	// the map data 
	// width of the map
	
	const char *fname = luaL_checkstring(l, 1); 
	int width = luaL_checkinteger(l, 4);
	lua_pop(l, 1);

	// read the array
	int num = lua_objlen(l, -1);

	if (num % width != 0)
		return luaL_error(l, "TileMap: Width must be a multiple of total tiles");

	// get the array
	int *cells = new int[num];
	readIntArray(l, cells, num);
	lua_pop(l, 1);

	Point tileSize = Point::pop(l);

	TileMap & tm = *(TileMap*)lua_newuserdata(l, sizeof(TileMap));

	if (!tm.load(fname, tileSize.y, tileSize.x, cells, width, num/width))
		return luaL_error(l, "TileMap: Failed to load tilesheet");

	if (luaL_newmetatable(l, "Map")) {
		lua_pop(l, 1);
		TileMap::install(l);
	}

	lua_setmetatable(l, -2);

	return 1;
}


// args
// 	Point viewport
int TileMap::_draw(lua_State *l)
{
	TileMap *map = (TileMap*)luaL_checkudata(l, 1, "Map");

	Point v = {0, 0};
	if (lua_gettop(l) == 2)
		v = Point::pop(l);

	map->draw(v);
	return 0;
}



int _width(lua_State *l)
{
	return 1;
}

int _height(lua_State *l)
{
	return 1;
}




