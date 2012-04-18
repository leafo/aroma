#pragma once

#include "common.h"
#include "lua_binding.h"
#include "renderer.h"

namespace aroma {
	class NaClTimeModule : public Bindable {
		protected:
			Renderer* renderer;

		public:
			NaClTimeModule(Renderer* renderer);
			static int _getTime(lua_State* l);
			static int _getFPS(lua_State* l);

			const char* module_name();
			void bind_all(lua_State* l);
	};
}
