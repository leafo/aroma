
#include "input.h"

namespace aroma {
	static const char* KEY_RELEASED_EVENT = "keyreleased";
	static const char* KEY_PRESSED_EVENT = "keypressed";

	InputHandler::InputHandler(LuaBinding* binding) : binding(binding) {
		for (int i = 32; i < 127; i++) {
			ascii_table[i - 32][0] = i;
			ascii_table[i - 32][1] = '\0';
		}

		binding->bind_module(this);
	}

	void InputHandler::dispatch_key_event(const char* event_name, int key) {
		lua_State* l = binding->lua();
		int top = lua_gettop(l);
		binding->push_self();

		const char* name = key_name(key);
		if (event_name == KEY_RELEASED_EVENT) {
			keys_down.erase(name);
		} else if (event_name == KEY_PRESSED_EVENT) {
			keys_down.insert(name);
		}

		lua_pushstring(l, name);
		lua_pushnumber(l, key);
		binding->send_event(event_name, 2);

		lua_settop(l, top);
	}

	void InputHandler::key_down(int key) {
		dispatch_key_event(KEY_PRESSED_EVENT, key);
	}

	void InputHandler::key_up(int key) {
		dispatch_key_event(KEY_RELEASED_EVENT, key);
	}

	bool InputHandler::is_key_down(const char* name) {
		return keys_down.find(name) != keys_down.end();
	}

	// TODO this is nacl specific right now, fix that
	const char* InputHandler::key_name(int key) {
		switch (key) {
			case 37: return "left";
			case 38: return "up";
			case 39: return "right";
			case 40: return "down";

			case 8: return "backspace";
			case 9: return "tab";

			case 13: return "return";
			case 16: return "lshift";
			case 17: return "lctrl";
			case 18: return "lalt";
			case 27: return "escape";

			case 112: return "f1";
			case 113: return "f2";
			case 114: return "f3";
			case 115: return "f4";
			case 116: return "f5";
			case 117: return "f6";
			case 118: return "f7";
			case 119: return "f8";
			case 120: return "f9";

			case 186: return ";";
			case 187: return "=";
			case 188: return ",";
			case 189: return "-";
			case 190: return ".";
		}

		if (key >= 32 && key <= 90) {
			if (key >= 'A' && key <= 'Z') key += 32; // make lowercase
			return ascii_table[key - 32];
		}

		return "unknown";
	}

	const char* InputHandler::module_name() {
		return "keyboard";
	}

	void InputHandler::bind_all(lua_State* l) {
		set_new_func("isDown", _isDown);
	}

	int InputHandler::_isDown(lua_State* l) {
		InputHandler* self = upvalue_self(InputHandler);
		lua_pushboolean(l, self->is_key_down(luaL_checkstring(l, 1)));
		return 1;
	}

}
