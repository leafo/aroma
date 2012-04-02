#pragma once

#include <vector>

#include "common.h"
#include "image.h"

using std::vector;

namespace aroma {
	struct Letter {
		int letter;
		int width;
	};

	class Font {
		protected:
			Image letters;
			int line_height;
			int start_i;
			vector<Letter> letter_map;

		public:
			Font(Image letters, int line_height, int start_i,
					vector<Letter> letter_map);

			void write_string(int x, int y, const char* str);

			int push(lua_State* l) const;

			static int _gc(lua_State* l);
			static int _print(lua_State* l);

			// TODO
			// set/get line height and letter spacing
			// width of string calculation
	};

	struct Glyph {
		int letter;
		byte* bytes;
		int width;
	};

	typedef vector<Glyph> GlyphList;

	// collects all the glyphs in one place before building the texture for the
	// font
	class GlyphCache {
		private:
			int height;
			GlyphList glyphs;

		public:
			GlyphCache();
			void add_glyph(int letter, byte* bytes, int w, int h);
			Font build_font();

			static int _new(lua_State* l);
			static int _add_glyph(lua_State* l);
			static int _to_font(lua_State* l);
			static int _gc(lua_State* l);
	};

	class FontModule : public Bindable {
		public:
			const char* module_name();
			void bind_all(lua_State* l);
	};

}
