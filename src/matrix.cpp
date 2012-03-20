
#include "matrix.h"

namespace aroma {

	Mat4 Mat4::identity() {
		Mat4 out = {
			1, 0, 0, 0, 
			0, 1, 0, 0, 
			0, 0, 1, 0, 
			0, 0, 0, 1,
		};
		return out;
	}

	Mat4 Mat4::ortho2d(float left, float right, float top, float bottom) {
		float near = -1;
		float far = 1;

		float tx = - (right + left) / (right - left);
		float ty = - (top + bottom) / (top - bottom);
		float tz = - (far + near) / (far - near);

		Mat4 out = {
			2.0/(right - left), 0, 0, 0, 
			0, 2.0/(top - bottom), 0, 0, 
			0, 0, -2.0/(far - near), 0, 
			tx, ty, tz, 1,
		};
		return out;
	}

	Mat4 Mat4::scale(float sx, float sy, float sz) {
		Mat4 out = {
			sx, 0, 0, 0, 
			0, sy, 0, 0, 
			0, 0, sz, 0, 
			0, 0, 0, 1,
		};
		return out;
	}


	Mat4 Mat4::translate(float tx, float ty, float tz) {
		Mat4 out = {
			1, 0, 0, 0, 
			0, 1, 0, 0, 
			0, 0, 1, 0, 
			tx, ty, tz, 1,
		};
		return out;
	}

	Mat4 Mat4::operator*(const Mat4 & other) const {
		// see lib/mult.moon
		Mat4 out = {
			data[0] * other.data[0] + data[1] * other.data[4] + data[2] * other.data[8] + data[3] * other.data[12],
			data[0] * other.data[1] + data[1] * other.data[5] + data[2] * other.data[9] + data[3] * other.data[13],
			data[0] * other.data[2] + data[1] * other.data[6] + data[2] * other.data[10] + data[3] * other.data[14],
			data[0] * other.data[3] + data[1] * other.data[7] + data[2] * other.data[11] + data[3] * other.data[15],
			data[4] * other.data[0] + data[5] * other.data[4] + data[6] * other.data[8] + data[7] * other.data[12],
			data[4] * other.data[1] + data[5] * other.data[5] + data[6] * other.data[9] + data[7] * other.data[13],
			data[4] * other.data[2] + data[5] * other.data[6] + data[6] * other.data[10] + data[7] * other.data[14],
			data[4] * other.data[3] + data[5] * other.data[7] + data[6] * other.data[11] + data[7] * other.data[15],
			data[8] * other.data[0] + data[9] * other.data[4] + data[10] * other.data[8] + data[11] * other.data[12],
			data[8] * other.data[1] + data[9] * other.data[5] + data[10] * other.data[9] + data[11] * other.data[13],
			data[8] * other.data[2] + data[9] * other.data[6] + data[10] * other.data[10] + data[11] * other.data[14],
			data[8] * other.data[3] + data[9] * other.data[7] + data[10] * other.data[11] + data[11] * other.data[15],
			data[12] * other.data[0] + data[13] * other.data[4] + data[14] * other.data[8] + data[15] * other.data[12],
			data[12] * other.data[1] + data[13] * other.data[5] + data[14] * other.data[9] + data[15] * other.data[13],
			data[12] * other.data[2] + data[13] * other.data[6] + data[14] * other.data[10] + data[15] * other.data[14],
			data[12] * other.data[3] + data[13] * other.data[7] + data[14] * other.data[11] + data[15] * other.data[15]
		};
		return out;
	}

}
