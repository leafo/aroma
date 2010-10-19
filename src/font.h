#ifndef FONT_H_
#define FONT_H_

#include "common.h"
#include "tiles.h"

class Font {
protected:
	Image letters;
	double letterWidth, letterHeight;	
	double letterSpan;
public:
	static int _new(lua_State *l);
	static int _string(lua_State *l); // draw string

	void string(const Point dest, const char *str) const;
};

#endif /* FONT_H_ */
