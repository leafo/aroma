
#include <iostream>
#include <fstream>

using namespace std;

#include "shader.h"

// read entire file
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


int Shader::_new(lua_State *l) {
	const char *frag_fname = luaL_checkstring(l, -1);
	const char *vert_fname = luaL_checkstring(l, -2);

	// attempt to read files
	char *vert_code = readFile(vert_fname);
	if (!vert_code) {
		// fail ?
	}
}

