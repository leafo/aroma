
#ifndef TILES_H_
#define TILES_H_

#include "common.h"
#include "image.h"
#include "geometry.h"

void register_Tiles(lua_State *l);

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

#endif /* TILES */

