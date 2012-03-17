
#pragma once

#include "common.h"

namespace aroma {

void register_Shader(lua_State *l);

class Shader {
public:
	bool linked;
	GLuint program;

	Shader();
	bool add(GLuint type, const char *src);
	bool link();
	void bind();

	GLuint attr_loc(const char* name);

	static int _new(lua_State *l);

	static int _bind(lua_State *l);
	static int _release(lua_State *l);
	static int _uniform(lua_State *l);

	static int _frag(lua_State *l);
	static int _vert(lua_State *l);

};

}

