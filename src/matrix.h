#pragma once

#include "common.h"
#include <stack>

using std::stack;

namespace aroma {
	class Shader;

	struct Mat4 {
		GLfloat data[16];

		Mat4 operator*(const Mat4 & other) const;

		void print();

		static Mat4 identity();
		static Mat4 ortho2d(float left, float right, float top, float bottom);
		static Mat4 scale(float sx=1, float sy=1, float sz=1);
		static Mat4 translate(float tx=0, float ty=0, float tz=0);
		static Mat4 rotate2d(float d);
	};

	class MatrixStack {
		protected:
			typedef stack<Mat4> mstack;
			mstack matrices;

		public:
			MatrixStack();

			void reset(Mat4 mat);
			void push(Mat4 mat);
			void push();
			void set(Mat4 mat);
			void mul(Mat4 mat);

			void pop();
			Mat4 current();

			void apply(Shader *shader, const char* name = "PMatrix");
	};

}
