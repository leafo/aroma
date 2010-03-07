
#ifndef SHADER_H_
#define SHADER_H_

#include "common.h"

class Shader {
public:
	GLuint vert;
	GLuint frag;
	GLuint program;

	static int _new(lua_State *l);

	static int _bind(lua_State *l);
	static int _release(lua_State *l);
};

#endif /* SHADER_H_ */

