
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

	void GlyphCache::add_glyph(int letter, ImageData data) {
		add_glyph(letter, data.bytes, data.width, data.height);
	}

	Font GlyphCache::build_font() {
		int max_width = 0;

		if (glyphs.size() == 0) {
			err("attemping to build font with 0 glyphs\n");
		}

		for (GlyphList::iterator g = glyphs.begin(); g != glyphs.end(); ++g) {
			if (g->width > max_width) max_width = g->width;
		}

		// find the power of two square texture that will fit all the glyphs, a little messy
		int tex_width = sqrt((max_width * height) * glyphs.size());
		if (tex_width < max_width) tex_width = max_width;
		if (tex_width < height) tex_width = height;
		tex_width = next_p2(tex_width);

		int per_row = tex_width / max_width;
		while((glyphs.size() / per_row + 1) * height > tex_width) {
			tex_width = next_p2(tex_width + 1);
			per_row = tex_width / max_width;
		}

		ImageData d(tex_width, tex_width);
		d.clear(Color(0,0,0,0));

		// write to image data
		int k = 0;

		LetterList letters;
		for (GlyphList::iterator i = glyphs.begin(); i != glyphs.end(); ++i) {
			Letter l = { i->letter, i->width };
			letters.push_back(l);

			int x = (k % per_row) * max_width;
			int y = (k / per_row) * height;

			d.update(x, y, ImageData(i->width, height, i->bytes));
			k++;
		}

		Image img = Image::from_data(d);
		d.free();

		return Font(img, height, max_width, letters);
	}

	int GlyphCache::_new(lua_State* l) {
		GlyphCache* self = newuserdata(GlyphCache);
		new (self) GlyphCache();

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
		// if created from lua then all the image data is being collected by lua
		return 0;
	}

	Font::Font(Image letter_tex, int line_height, int max_w, LetterList letters)
		: letter_tex(letter_tex),
			line_height(line_height),
			max_width(max_w),
			letters(letters)
		{
			int max_char = 0;
			int min_char = -1;

			for (LetterList::iterator l = letters.begin(); l != letters.end(); ++l) {
				if (l->letter > max_char) max_char = l->letter;
				if (min_char == -1 || l->letter < min_char) min_char = l->letter;
			}

			letter_map = vector<int>(max_char - min_char + 1);

			int count = letters.size();
			for (int i = 0; i < count; i++) {
				letter_map[letters[i].letter - min_char] = i;
			}

			start_i = min_char;
		}

	int Font::push(lua_State* l) const {
		Font* self = newuserdata(Font);
		new (self) Font(*this); // AWESOME!

		if (luaL_newmetatable(l, "Font")) {
			lua_newtable(l);
			// setfunction("print", _print);
			lua_setfield(l, -2, "__index");
			setfunction("__gc", _gc);
		}
		lua_setmetatable(l, -2);

		return 1;
	}


	int Font::_new_image_font(lua_State *l) {
		ImageData* data = getself(ImageData);
		const char *letters = luaL_checkstring(l, 2);
		size_t num_letters = strlen(letters);

		int line_height = data->height;

		Color* pixels = (Color*)data->bytes;
		Color spacer = pixels[0];

		vector<ImageData> glyph_data;
		GlyphCache glyphs;

		int letter_i = 0;
		int x = 0;

		int x_start = -1;

		while (x < data->width) {
			if (pixels[x] == spacer) {
				if (x_start != -1) {
					// found something
					ImageData slice = data->slice(x_start, 0, x - x_start, line_height);
					glyph_data.push_back(slice);
					glyphs.add_glyph(letters[letter_i++], slice);

					if (letter_i == num_letters) {
						break; // no more letters
					}
				}

				x_start = -1;
			} else if (x_start == -1) {
				x_start = x;
			}

			x++;
		}

		Font f = glyphs.build_font();

		// throw out the glyph data
		for (int k = 0; k < glyph_data.size(); k++) {
			glyph_data[k].free();
		}

		return f.push(l);
	}

	int Font::_gc(lua_State* l) {
		Font* self = getself(Font);
		self->letter_tex.free();
		return 0;
	}

	const char* FontModule::module_name() {
		return "font";
	}

	void FontModule::bind_all(lua_State* l) {
		setfunction("newGlyphCache", GlyphCache::_new);
	}

}

