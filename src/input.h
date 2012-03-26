#pragma once

#include "common.h"
#include "lua_binding.h"

#include <set>

using std::set;

namespace aroma {
	class InputHandler : public Bindable {
		protected:
			LuaBinding* binding;
			void dispatch_key_event(const char* func, int key);
			char ascii_table[95][2];
			set<std::string> keys_down;

		public:
			const char* key_name(int key);

			InputHandler(LuaBinding* binding);

			void key_down(int key);
			void key_up(int key);

			bool is_key_down(const char* name);

			// mousedown, mouseup, mousemove, etc
			
			const char* module_name();
			void bind_all(lua_State *l);

			static int _isDown(lua_State *l);
	};
}
