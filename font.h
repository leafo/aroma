
#ifndef FONT_H_
#define FONT_H_

#include "common.h"
#include "tiles.h"

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


#endif /* FONT_H_ */
