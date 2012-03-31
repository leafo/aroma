#pragma once

#include "common.h"
#include "lua_binding.h"

namespace aroma {
	class AudioModule : public Bindable {
		public:
			const char* module_name();
			void bind_all(lua_State* l);
	};

	struct AudioSource {
		virtual void play() = 0;
	};
}
