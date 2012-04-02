
#pragma once

#include "common.h"
#include "image.h"
#include "geometry.h"

namespace aroma {
	class Tiled {
		protected:
			Image image;
			int tw, th, count;

		public:
			Tiled(Image i, int tw, int th, int count);
			void draw_tile(int x, int y, int i);
	};

}

