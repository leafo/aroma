#pragma once

#include "common.h"

namespace aroma {
	class Bindable {
		public:
			virtual const char* module_name() = 0;
			virtual void bind_all(lua_State* l) = 0;
	};

	class LuaBinding {
		protected:
			lua_State* l;
			lua_State* game_thread;
			void preload_library(const char* name);
			virtual void handle_error(lua_State *thread, const char* name);

		public:
			LuaBinding();
			lua_State* lua();

			virtual void bind_module(Bindable *b);
			virtual bool bind_all(); // set up the lua env

			bool is_type(lua_State* l, int i, const char* type);

			void set_game_thread(lua_State* thread);

			// pops nargs from top and send event to name on game_thread
			void send_event(const char* name, int nargs);

			bool load_and_run(void* buffer, size_t buff_len, const char* name);
			void push_self();
	};

	void stack_dump(lua_State *L);
}

