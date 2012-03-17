#pragma once

namespace aroma {
	class GLContext {
		public:
			virtual bool make_current() = 0;
			virtual void resize(const int w, const int h) = 0;
			virtual void flush() = 0;
			// virtual void render(); // tell the renderer to tick

			virtual int width() = 0;
			virtual int height() = 0;
	};

#ifndef AROMA_NACL

	class GLFWContext : public GLContext {
		protected:
			bool created;
			int w;
			int h;
			const char* title;

			bool make_window();

		public:
			GLFWContext(int w, int h, const char* title);

			bool make_current();
			void resize(const int w, const int h);
			void flush();

			int width();
			int height();
	};

#endif

}

