
#include <iostream>
#include <fstream>

using namespace std;

#include "shader.h"
#include "image.h"

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
		setfunction("uniform", Shader::_uniform);

		lua_setfield(l, -2, "__index");
	}
	lua_setmetatable(l, -2);

	return 1;
}


int Shader::_bind(lua_State *l) {
	Shader *s = (Shader*)luaL_checkudata(l, 1, "Shader");
	glUseProgram(s->program);

	if (LUA_TTABLE == lua_type(l, -1)) {
		return _uniform(l);
	}

	return 0;
}

int Shader::_release(lua_State *l) {
	glUseProgram(0);
	return 0;
}


/**
 * set some uniform values
 */
int Shader::_uniform(lua_State *l) {
	Shader *self = getself(Shader);
	lua_pushnil(l);
	while (lua_next(l, -2) != 0) {
		const char *key = luaL_checkstring(l, -2);
		GLuint uloc = glGetUniformLocation(self->program, key);

		int type = lua_type(l, -1);
		switch (type) {
			case LUA_TNUMBER:
				glUniform1f(uloc, luaL_checknumber(l, -1));
				break;
			case LUA_TUSERDATA: {
				// this is wrong: it is based on texture unit
				Image *image = (Image*)luaL_checkudata(l, -1, "Image");
				if (image) {
					glUniform1i(uloc, image->texid);
				}
				break;
			}
			default:
				cout << "unknown uniform type: " << type << endl;
		}

		lua_pop(l, 1);
	}

	return 0;
}

