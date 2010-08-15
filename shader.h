
#ifndef SHADER_H_
#define SHADER_H_

#include "common.h"

class Shader {
public:
	bool linked;
	GLuint program;

	Shader();
	bool add(GLuint type, const char *src);

	static int _new(lua_State *l);

	static int _bind(lua_State *l);
	static int _release(lua_State *l);
	static int _uniform(lua_State *l);

	static int _frag(lua_State *l);
	static int _vert(lua_State *l);

};

#endif /* SHADER_H_ */

