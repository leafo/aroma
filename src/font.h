#pragma once

#include "common.h"
#include <map>
#include <vector>

using std::map;
using std::vector;

namespace aroma {
	class Font {
	};

	struct Glyph {
		int letter;
		byte* bytes;
		size_t width;
	};

	typedef vector<Glyph> GlyphList;

	// collects all the glyphs in one place before building the texture for the font
	class GlyphCache {
		private:
			size_t height;
			GlyphList glyphs;

		public:
			GlyphCache();
			void add_glyph(int letter, byte* bytes, int w, int h);
			Font build_font();
	};

}
