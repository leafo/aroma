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

void dumpProgramLog(GLuint program) {
	int len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		int written;
		char log[len];
		glGetProgramInfoLog(program, len, &written, log);
		printf("%s", log);
	}
}

Shader::Shader() : linked(false) {
}

/**
 * load shader source form arguments
 */
int Shader::_new(lua_State *l) {
	int argc = lua_gettop(l);

	Shader s;
	s.program = glCreateProgram();

	bool success = true;
	if (argc > 0) {
		cout << "compiling fragment shader" << endl;
		const char *frag = luaL_checkstring(l, -1);
		success &= s.add(GL_FRAGMENT_SHADER, frag);
	}

	if (argc > 1) {
		cout << "compiling vertex shader" << endl;
		const char *vert = luaL_checkstring(l, -2);
		success &= s.add(GL_VERTEX_SHADER, vert);
	}

	if (!success)
		return luaL_error(l, "Shader: failed to compile");

	*newuserdata(Shader) = s;

	if (luaL_newmetatable(l, "Shader")) {
		lua_newtable(l);		

		setfunction("bind", Shader::_bind);
		setfunction("release", Shader::_release);
		setfunction("uniform", Shader::_uniform);
		setfunction("vert", Shader::_vert);
		setfunction("frag", Shader::_frag);

		lua_setfield(l, -2, "__index");
	}
	lua_setmetatable(l, -2);

	return 1;
}

int Shader::_frag(lua_State *l) {
	Shader *self = getself(Shader);
	const char *src = luaL_checkstring(l, -1);
	if (!self->add(GL_FRAGMENT_SHADER, src)) {
		return luaL_error(l, "Shader: failed to compile fragment shader");
	}

	return 0;
}

int Shader::_vert(lua_State *l) {
	Shader *self = getself(Shader);
	const char *src = luaL_checkstring(l, -1);
	if (!self->add(GL_VERTEX_SHADER, src)) {
		return luaL_error(l, "Shader: failed to compile fragment shader");
	}
	return 0;
}


int Shader::_bind(lua_State *l) {
	Shader *self = getself(Shader);
	if (!self->linked) {
		glLinkProgram(self->program);
		int success;
		glGetProgramiv(self->program, GL_LINK_STATUS, &success);
		if (!success) {
			dumpProgramLog(self->program);
			luaL_error(l, "Shader: failed to link shader");
		}

		self->linked = true;
	}

	glUseProgram(self->program);

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

bool Shader::add(GLuint type, const char *src) {
	if (linked) return false;
	GLuint sid = glCreateShader(type);
	glShaderSource(sid, 1, &src, NULL);

	glCompileShader(sid);
	dumpShaderLog(sid);

	int compiled;
	glGetShaderiv(sid, GL_COMPILE_STATUS, &compiled);
	if (!compiled) return false;

	glAttachShader(program, sid);

	return true;
}

