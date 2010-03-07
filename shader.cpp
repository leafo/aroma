
#include <iostream>
#include <fstream>

using namespace std;

#include "shader.h"

// read entire file
// probably not going to use this
char* readFile(const char *fname) {
	int length;
	ifstream in(fname);
	if (!in) return 0;

	in.seekg(0, ios::end);
	length = in.tellg();
	in.seekg(0, ios::beg);

	char *buffer = new char[length];
	in.read(buffer, length);
	in.close();

	return buffer;
}

void dumpShaderLog(GLuint shader) {
	int len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		int written;
		char log[len];
		glGetShaderInfoLog(shader, len, &written, log);
		printf("%s", log);
	}
}


/**
 * load shader source form arguments
 */
int Shader::_new(lua_State *l) {
	const char *frag = luaL_checkstring(l, -1);
	const char *vert = luaL_checkstring(l, -2);

	// attempt to compile the shaders
	Shader s;
	s.vert = glCreateShader(GL_VERTEX_SHADER);
	s.frag = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(s.vert, 1, &vert, NULL);
	glShaderSource(s.frag, 1, &frag, NULL);

	glCompileShader(s.vert);
	dumpShaderLog(s.vert);

	glCompileShader(s.frag);
	dumpShaderLog(s.frag);

	// build the program
	s.program = glCreateProgram();
	glAttachShader(s.program, s.vert);
	glAttachShader(s.program, s.frag);
	glLinkProgram(s.program);

	*(Shader*)lua_newuserdata(l, sizeof(Shader)) = s;

	if (luaL_newmetatable(l, "Shader")) {
		lua_newtable(l);		

		setfunction("bind", Shader::_bind);
		setfunction("release", Shader::_release);

		lua_setfield(l, -2, "__index");
	}
	lua_setmetatable(l, -2);

	return 1;
}


int Shader::_bind(lua_State *l) {
	Shader *s = (Shader*)luaL_checkudata(l, 1, "Shader");
	glUseProgram(s->program);
	return 0;
}

int Shader::_release(lua_State *l) {
	glUseProgram(0);
	return 0;
}

