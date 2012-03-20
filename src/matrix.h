#pragma once

#include "common.h"

namespace aroma {
	struct Mat4 {
		GLfloat data[16];

		Mat4 operator*(const Mat4 & other) const;

		static Mat4 identity();
		static Mat4 ortho2d(float left, float right, float top, float bottom);
		static Mat4 scale(float sx=1, float sy=1, float sz=1);
		static Mat4 translate(float tx=1, float ty=1, float tz=1);
	};
}
