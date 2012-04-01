
#include <cmath>
#include "font.h"
// #include "image.h"

namespace aroma {
	// int next_p2(int x) {
	// 	int size = 1;
	// 	while (size < x) {
	// 		size = size << 1;
	// 	}
	// 	return size;
	// }

	GlyphCache::GlyphCache() : height(0) {
	}

	void GlyphCache::add_glyph(int letter, byte* bytes, int w, int h) {
		if (height == 0) {
			height = h;
		} else if (height != h) {
			err("mismatched glyph height\n");
		}

		Glyph g = { letter, bytes, w };
	}

	Font GlyphCache::build_font() {
		int max_width = 0;
		int max_char = 0;
		int min_char = -1;

		for (GlyphList::iterator i = glyphs.begin(); i != glyphs.end(); ++i) {
			if (i->width > max_width) max_width = i->width;
			if (i->letter > max_char) max_char = i->letter;
			if (min_char == -1 || i->letter < min_char) min_char = i->letter;
		}

		// pick a texture size
		// int tex_width = next_p2(sqrt((max_width * height) * glyphs.size()));

		// create an image data, and blit into each block the font

		return Font();
	}
}

