#pragma once

#include "common.h"

namespace aroma {
	class LuaBinding {
		protected:
			lua_State* l;
			void preload_library(const char* name);

		public:
			LuaBinding();
			lua_State* lua();

			virtual bool bind_all(); // set up the lua env
	};
}


