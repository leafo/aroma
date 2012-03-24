
#include "shader.h"
#include "image.h"

namespace aroma {

void register_Shader(lua_State *l) {
	setfunction("shader", Shader::_new);
}

void dumpShaderLog(GLuint shader) {
	int len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		int written;
		char log[len];
		glGetShaderInfoLog(shader, len, &written, log);
		fprintf(stderr, "%s\n", log);
	}
}

void dumpProgramLog(GLuint program) {
	int len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		int written;
		char log[len];
		glGetProgramInfoLog(program, len, &written, log);
		fprintf(stderr, "%s\n", log);
	}
}

Shader::Shader() : linked(false) {
	program = glCreateProgram();
}

// create a new shader
// aroma.graphics.newShader([vertex_src], frag_src);
int Shader::_new(lua_State *l) {
	int argc = lua_gettop(l);

	Shader s;
	bool success = true;
	if (argc > 0) {
		log("compiling fragment shader\n");
		const char *frag = luaL_checkstring(l, -1);
		success &= s.add(GL_FRAGMENT_SHADER, frag);
	}

	if (argc > 1) {
		log("compiling vertex shader\n");
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
	if (!self->linked && !self->link()) {
		luaL_error(l, "Shader: failed to link shader");
	}

	self->bind();


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
		GLuint uloc = self->uniform_loc(key);

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
				log("unknown uniform type: %d\n", type);
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

bool Shader::link() {
	if (linked) return true;

	glLinkProgram(program);
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		dumpProgramLog(program);
		return false;
	}

	linked = true;
	return true;
}

void Shader::bind() {
	glUseProgram(program);
}

GLuint Shader::attr_loc(const char* name) {
	return glGetAttribLocation(program, name);
}

GLuint Shader::uniform_loc(const char* name) {
	return glGetUniformLocation(program, name);
}

void Shader::set_uniform(const char* name, const Mat4 & matrix) {
	GLuint loc = glGetUniformLocation(program, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&matrix);
}

void Shader::set_uniform(const char* name, const Color & color) {
	GLuint loc = glGetUniformLocation(program, name);
	glUniform4f(loc, color.rf(), color.gf(), color.bf(), color.af());
}

void Shader::set_uniform(const char* name, const float num) {
	GLuint loc = glGetUniformLocation(program, name);
	glUniform1f(loc, num);
}

void Shader::set_uniform(const char* name, const int num) {
	GLuint loc = glGetUniformLocation(program, name);
	glUniform1i(loc, num);
}

}
