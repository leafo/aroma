#include "image.h"
#include <corona.h>

static int texCoords[] = {
	0,0,  1,0,  1,1,  0,1
};

static int next_p2(int x);

struct Pixel {
	unsigned char r, g, b, a;
};

int Image::create(int width, int height, const void *bytes) {
	this->width = width;
	this->height = height;

	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, bytes);	
}

int Image::load(const char *fname)
{
	double start = glfwGetTime();
	corona::Image *tmp = corona::OpenImage(fname, corona::PF_R8G8B8A8);

	if (!tmp) return 0; // failed

	int width = tmp->getWidth();
	int height = tmp->getHeight();

	// find a color key
	Pixel *pixels = (Pixel*)tmp->getPixels();
	for (int i = 0; i < width*height; i++) {
		if (pixels->r == 255 &&
				pixels->g == 0 &&
				pixels->b == 255)
		{
			pixels->a = 0;
		}
		// cout << (int)pixels->a << endl;
		pixels++;
	}

	cout << "loading image: " << glfwGetTime() - start << endl;
	start = glfwGetTime();

	create(width, height, tmp->getPixels());
	cout << "uploading image: " << glfwGetTime() - start << endl;

	delete tmp;
	return 1;
}

void Image::draw(int x, int y) {
	/*
	float coords[] = {
		x, y,
		x+width, y,
		x+width, y+height,
		x, y+height
	};
	*/

	glBindTexture(GL_TEXTURE_2D, texid);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2d(x, y);
		glTexCoord2f(1, 0);
		glVertex2d(x + width, y);
		glTexCoord2f(1, 1);
		glVertex2d(x + width, y + height);
		glTexCoord2f(0, 1);
		glVertex2d(x, y + height);
	glEnd();
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

int Image::_new(lua_State *l) {
	const char *fname = luaL_checkstring(l, 1);

	Image *i = newuserdata(Image);

	if (!i->load(fname))
		return luaL_error(l, "Failed to load image: %s", fname);

	if (luaL_newmetatable(l, "Image")) {
		lua_newtable(l); // the index table
		setfunction("draw", Image::_draw);
		setfunction("blit", Image::_blit);

		lua_setfield(l, -2, "__index");
	}

	lua_setmetatable(l, -2);

	return 1;
}


int Image::_draw(lua_State *l) {
	Point p = Point::pop(l); 
	getself(Image)->draw(p.x, p.y);
	return 0;
}

int Image::_blit(lua_State *l) {
	Rect dest = Rect::pop(l);
	Rect src = Rect::pop(l);

	getself(Image)->blit(src, dest);
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

