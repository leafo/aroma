#include "common.h"
#include <corona.h>

static int next_p2(int x);

struct Pixel {
	unsigned char r, g, b, a;
};

int Image::load(const char *fname)
{
	corona::Image *tmp = corona::OpenImage(fname, corona::PF_R8G8B8A8);

	if (!tmp) return 0; // failed

	width = tmp->getWidth();
	height = tmp->getHeight();

	// find a color key
	Pixel *pixels = (Pixel*)tmp->getPixels();
	for (int i = 0; i < width*height; i++)
	{
		if (pixels->r == 255 &&
				pixels->g == 0 &&
				pixels->b == 255)
		{
			pixels->a = 0;
		}
		// cout << (int)pixels->a << endl;
		pixels++;
	}


	glGenTextures(1, &(texid));
	glBindTexture(GL_TEXTURE_2D, texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, tmp->getPixels());

	delete tmp;

	return 1;
}

void Image::blit(Rect src, Rect dest)
{
	src.x = src.x / width;
	src.y = src.y / height;

	src.w = src.w / width;
	src.h = src.h / height;

	glBindTexture(GL_TEXTURE_2D, texid);
	glBegin(GL_QUADS);
		glTexCoord2f(src.x, src.y);
		glVertex2d(dest.x, dest.y);
		glTexCoord2f(src.x + src.w, src.y);
		glVertex2d(dest.x + dest.w, dest.y);
		glTexCoord2f(src.x + src.w, src.y + src.h);
		glVertex2d(dest.x + dest.w, dest.y + dest.h);
		glTexCoord2f(src.x, src.y + src.h);
		glVertex2d(dest.x, dest.y + dest.h);
	glEnd();
}


void Image::push(lua_State *l, Image *i)
{
}

Image Image::pop(lua_State *l)
{
	Image *i = (Image*)luaL_checkudata(l, -1, "Image");
	lua_pop(l, 1);
	return *i;
}

void Image::install(lua_State *l)
{
	luaL_newmetatable(l, "Image");

	lua_newtable(l); // the index table
	setfunction("draw", Image::_draw);
	setfunction("blit", Image::_blit);

	lua_setfield(l, -2, "__index");
}

int Image::_new(lua_State *l)
{
	const char *fname = luaL_checkstring(l, 1);

	Image i;
	// CHANGE THIS
	if (!i.load(fname))
		return luaL_error(l, "Failed to load image: %s", fname);

	*(Image*)lua_newuserdata(l, sizeof(Image)) = i;

	if (luaL_newmetatable(l, "Image")) {
		lua_pop(l, 1);
		Image::install(l);
	}

	lua_setmetatable(l, -2);

	return 1;
}


int Image::_draw(lua_State *l)
{
	Point loc = Point::pop(l); 
	Image image = Image::pop(l);

	glBindTexture(GL_TEXTURE_2D, image.texid);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2d(loc.x, loc.y);
		glTexCoord2f(1, 0);
		glVertex2d(loc.x + image.width, loc.y);
		glTexCoord2f(1, 1);
		glVertex2d(loc.x + image.width, loc.y + image.height);
		glTexCoord2f(0, 1);
		glVertex2d(loc.x, loc.y + image.height);
	glEnd();	

	return 0;
}
int Image::_blit(lua_State *l)
{
	Rect dest = Rect::pop(l);
	Rect src = Rect::pop(l);
	Image image = Image::pop(l);

	image.blit(src, dest);
	return 0;
}

static int next_p2(int x)
{
	int size = 1;
	while (size < x) {
		size = size << 1;
	}
	return size;
}

