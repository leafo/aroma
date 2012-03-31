
#include "audio.h"

namespace aroma {
	const char* AudioModule::module_name() {
		return "audio";
	}

	void AudioModule::bind_all(lua_State* l) {
	}
}
