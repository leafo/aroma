
#include "font.h"

typedef struct {
	double x1,y1, x2,y2, x3,y3, x4,y4;

	void alignedSet(double x, double y, double w, double h) {
		x1 = x4 = x;
		x2 = x3 = x+w;

		y1 = y2 = y;
		y3 = y4 = y+h;
	}
	
	void print() {
		cout << "quad2d [" << $(x1) << $(y1) << $(x2) << $(y2)
			<< $(x3) << $(y3) << $(x4) << $(y4) << "]" << endl;
	}

} Quad2d;

int Font::_new(lua_State *l) {
	Point lsize = Point::pop(l); // letter width
	const char *fname = luaL_checkstring(l, -1);

	Font *self = newuserdata(Font);

	self->letters.load(fname);
	self->letterWidth = lsize.x;
	self->letterHeight = lsize.y;
	self->letterSpan = 7;

	if (luaL_newmetatable(l, "Font")) {
		setfunction("__call", Font::_string);
	}

	lua_setmetatable(l, -2);

	return 1;
}

int Font::_string(lua_State *l) {
	const Font *self = getself(Font);
	const char *str = luaL_checkstring(l, -1); lua_pop(l, 1);
	Point dest = Point::pop(l);
	self->string(dest, str);
	return 0;
}

void Font::string(const Point dest, const char *str) const {
	const int xstride = letters.width/letterWidth; // letters per row in texture
	const int ystride = letters.height/letterHeight;

	int len = strlen(str);
	Quad2d verts[len];
	Quad2d texcoords[len];

	double x = dest.x;
	double y = dest.y;

	double ds = letterWidth/letters.width;
	double dt = letterHeight/letters.height;

	int k = 0; // count of written letters
	char c;
	while (c = *str++) {
		switch (c) {
			case '\n':
				y += letterHeight;
				x = dest.x;
				break;
			default:
				verts[k].alignedSet(x,y, letterWidth, letterHeight);
				texcoords[k].alignedSet((double)(c%xstride)/xstride, (double)(c/xstride)/ystride, ds, dt);
				x += letterSpan;
				k++;
		}
	}

	letters.bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_DOUBLE, 0, verts);
	glTexCoordPointer(2, GL_DOUBLE, 0, texcoords);

	glVertexPointer(2, GL_DOUBLE, 0, verts);

	glDrawArrays(GL_QUADS, 0, k*4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

