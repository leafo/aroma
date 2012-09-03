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

	struct Glyph {
		int letter;
		byte* bytes;
		int width;
	};

	typedef vector<Letter> LetterList;
	typedef vector<Glyph> GlyphList;

	class Font {
		protected:
			Image letter_tex;
			int line_height, max_width;
			int start_i;
			LetterList letters;
			vector<int> letter_map;

		public:
			friend class Renderer;

			Font(Image letter_tex, int line_height, int max_w, LetterList letters);

			int push(lua_State* l) const;

			static int _new_image_font(lua_State *l);
			static int _gc(lua_State* l);
			static int _print(lua_State* l);

			// TODO
			// set/get line height and letter spacing
			// width of string calculation
	};

	// collects all the glyphs in one place before building the texture for the
	// font
	class GlyphCache {
		private:
			int height;
			GlyphList glyphs;

		public:
			GlyphCache();
			void add_glyph(int letter, byte* bytes, int w, int h);
			void add_glyph(int letter, ImageData data);

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
