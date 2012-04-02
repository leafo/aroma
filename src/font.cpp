
#include <cmath>
#include "font.h"
#include "renderer.h"

namespace aroma {

	GlyphCache::GlyphCache() : height(0) {
	}

	void GlyphCache::add_glyph(int letter, byte* bytes, int w, int h) {
		if (height == 0) {
			height = h;
		} else if (height != h) {
			err("mismatched glyph height, got %d, expecting %d\n", h, height);
		}

		Glyph g = { letter, bytes, w };
		glyphs.push_back(g);
	}

	Font GlyphCache::build_font() {
		int max_width = 0;
		int max_char = 0;
		int min_char = -1;

		if (glyphs.size() == 0) {
			err("attemping to build font with 0 glyphs\n");
		}

		for (GlyphList::iterator g = glyphs.begin(); g != glyphs.end(); ++g) {
			if (g->width > max_width) max_width = g->width;
			if (g->letter > max_char) max_char = g->letter;
			if (min_char == -1 || g->letter < min_char) min_char = g->letter;
		}

		// I don't know if this even works
		int tex_width = sqrt((max_width * height) * glyphs.size());
		if (tex_width < max_width) tex_width = max_width;
		if (tex_width < height) tex_width = height;
		tex_width = next_p2(tex_width);

		ImageData d(tex_width, tex_width);
		d.clear(Color(0,0,0,0));

		vector<Letter> letter_map(max_char - min_char + 1);

		// write to image data
		int per_row = tex_width / max_width;
		int k = 0;

		for (GlyphList::iterator i = glyphs.begin(); i != glyphs.end(); ++i) {
			Letter l = { i->letter, i->width };
			letter_map[i->letter - min_char] = l;

			int x = (k % per_row) * max_width;
			int y = (k / per_row) * height;

			d.update(x, y, ImageData(i->width, height, i->bytes));
			k++;
		}

		Image img = Image::from_data(d);
		d.free();

		return Font(img, height, min_char, letter_map);
	}

	int GlyphCache::_new(lua_State* l) {
		GlyphCache* self = newuserdata(GlyphCache);

		GlyphCache tmp;
		memcpy(self, (const void*)&tmp, sizeof(GlyphCache));

		if (luaL_newmetatable(l, "GlyphCache")) {
			lua_newtable(l);

			setfunction("add_glyph", _add_glyph);
			setfunction("to_font", _to_font);

			lua_setfield(l, -2, "__index");

			setfunction("__gc", _gc);
		}
		lua_setmetatable(l, -2);

		return 1;
	}

	// add_glyph(GlyphCache, letter:number, image:ImageData)
	int GlyphCache::_add_glyph(lua_State* l) {
		GlyphCache* self = getself(GlyphCache);
		int letter = luaL_checknumber(l, 2);
		ImageData* data = getselfi(ImageData, 3);
		self->add_glyph(letter, data->bytes, data->width, data->height);
		return 0;
	}

	int GlyphCache::_to_font(lua_State* l) {
		GlyphCache* self = getself(GlyphCache);
		return self->build_font().push(l);
	}

	int GlyphCache::_gc(lua_State* l) {
		log("collecting glyph cache\n");
		// if created from lua then all the image data is being collected by lua
		return 0;
	}

	Font::Font(Image letters, int line_height, int start_i,
			vector<Letter> letter_map) :
		letters(letters), line_height(line_height), start_i(start_i),
		letter_map(letter_map) { }

	void Font::write_string(int x, int y, const char* str) {
		size_t len = strlen(str);
		QuadCoords verts[len];
		QuadCoords tex[len];

		for (int i = 0; i < len; i++) {
			const Letter l = letter_map[str[i] - start_i];
			verts[i] = QuadCoords::from_rect(x, y, l.width, line_height);
			x += l.width;
			// tex[i] = QuadCoords::from_rect(x, y);
		}
	}

	int Font::push(lua_State* l) const {
		Font* self = newuserdata(Font);
		// *self = *this; // doesn't work with std containers
		memcpy(self, this, sizeof(Font));

		if (luaL_newmetatable(l, "Font")) {
			lua_newtable(l);
			setfunction("print", _print);
			lua_setfield(l, -2, "__index");
			setfunction("__gc", _gc);
		}
		lua_setmetatable(l, -2);

		return 1;
	}

	int Font::_print(lua_State* l) {
		return 0;
	}

	int Font::_gc(lua_State* l) {
		Font* self = getself(Font);
		self->letters.free();
		return 0;
	}

	const char* FontModule::module_name() {
		return "font";
	}

	void FontModule::bind_all(lua_State* l) {
		setfunction("newGlyphCache", GlyphCache::_new);
	}

}

