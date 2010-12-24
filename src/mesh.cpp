
#include "mesh.h"

int Mesh::_new(lua_State *l) {
	int dim = 3;
	GLenum format = GL_TRIANGLES;

	// handle additional arguments
	switch(lua_gettop(l)) {
		case 3: { // check for draw type
			const char *type = luaL_checkstring(l, -1);
			if (strcmp(type, "quads") == 0)
				format = GL_QUADS;	
			else if (strcmp(type, "triangles") == 0)
				format = GL_TRIANGLES;	
			else if (strcmp(type, "polygon") == 0)
				format = GL_POLYGON;	
			else if (strcmp(type, "triangle_strip") == 0)
				format = GL_TRIANGLE_STRIP;	
			lua_pop(l, 1);

		} case 2: { // check for dimension
			dim = luaL_checkinteger(l, -1);
			lua_pop(l, 1);
		}
	}

	int count = lua_objlen(l, -1);
	if (count % dim != 0) {
		return luaL_error(l, "invalid number of vertices for dimension: %d", dim);
	}

	double *v = new double[count];
	readArray(l, v, count);

	Mesh *m = (Mesh*)lua_newuserdata(l, sizeof(Mesh));

	printf("created mesh %p of dimension %d with %d vertices\n", m, dim, count/dim);

	if (luaL_newmetatable(l, "Mesh")) {
		lua_newtable(l);		
		setfunction("render", Mesh::_render);
		setfunction("dump", Mesh::_dump);
		setfunction("setNormals", Mesh::_setNormals);
		setfunction("vertices", Mesh::_vertices);

		setfunction("count", Mesh::_count);
		setfunction("dim", Mesh::_dim);

		setfunction("set", Mesh::_set);
		setfunction("get", Mesh::_get);

		setfunction("clone", Mesh::_clone);

		lua_setfield(l, -2, "__index");

		setfunction("__gc", Mesh::_gc);
	}
	lua_setmetatable(l, -2);

	m->format = format;
	m->dim = dim;
	m->count = count/dim;
	m->vertexArray = v;
	m->normalArray = 0;

	// calculate the normals? if we are a triangle
	/*
	if (dim == 3 && format == GL_TRIANGLES) {
		printf("calculating normals for mesh\n");
		m->normalArray = new double[count];
		for (int f = 0; f < m->count; f++) {
			Point v1 = Point::fromArray(m->vertexArray+f*dim,dim);
			Point v2 = Point::fromArray(m->vertexArray+f*dim+1,dim);
			Point v3 = Point::fromArray(m->vertexArray+f*dim+2,dim);
			v2.subtract(v1).print();

			// assuming they are counterclockwise	
			struct Three { double a[3]; };

			Point n = v2.subtract(v1).cross(v3.subtract(v1));

			memcpy(m->normalArray+f*dim, &n, 3*sizeof(double));
			memcpy(m->normalArray+f*dim+1, &n, 3*sizeof(double));
			memcpy(m->normalArray+f*dim+1, &n, 3*sizeof(double));
		}
	}
	*/

	return 1;
}

int Mesh::_setNormals(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");

	int count = lua_objlen(l, -1);
	if (count % m->dim != 0) {
		return luaL_error(l, "invalid number of vertices for dimension: %d", m->dim);
	}

	double *normals = new double[count];
	readArray(l, normals, count);

	if (m->normalArray) delete m->normalArray;
	m->normalArray = normals;
	
	return 0;
}


int Mesh::_render(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");

	// if there is a color
	bool setColor = false;
	if (lua_gettop(l) > 1) {
		Color c = Color::pop(l);
		c.bind();
		setColor = true;
	}

	glVertexPointer(m->dim, GL_DOUBLE, 0, m->vertexArray);
	glEnableClientState(GL_VERTEX_ARRAY);

	if (m->normalArray) {
		glNormalPointer(GL_DOUBLE, 0, m->normalArray);
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	glDrawArrays(m->format, 0, m->count);

	if (m->normalArray) glDisableClientState(GL_NORMAL_ARRAY);
	// glDisable(GL_VERTEX_ARRAY); // we always use this during draw arrays

	if (setColor) Color::White.bind();		

	return 0;
}

int Mesh::_dump(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");
	printf("Mesh with %d vertices of dimension %d\n",
			m->count, m->dim);

	return 0;
}

int Mesh::_gc(lua_State *l) {
	printf("mesh is being collected\n");
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");
	delete m->vertexArray;
	if (m->normalArray) delete m->normalArray;
	return 0;
}

// returns a vertex iterator
int Mesh::_vertices(lua_State *l) {
	/*Mesh *m = (Mesh*)*/luaL_checkudata(l, 1, "Mesh");

	lua_pushvalue(l, 1); // copy the mesh
	lua_pushinteger(l, 0); // the counter
	lua_pushcclosure(l, Mesh::_verticesIter, 2);

	return 1;
}

int Mesh::_verticesIter(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, lua_upvalueindex(1), "Mesh");
	int i = luaL_checkinteger(l, lua_upvalueindex(2));

	if (i+1 > m->count) return 0; // done

	lua_pushinteger(l, i+1);
	lua_replace(l, lua_upvalueindex(2));

	Point *p = (Point*)(m->vertexArray+m->dim*i); // get the ith vertex

	lua_pushinteger(l, i+1);
	lua_pushnumber(l, p->x);
	lua_pushnumber(l, p->y);
	if (m->dim > 2) lua_pushnumber(l, p->z);

	return m->dim + 1;
}

// set the ith vertex
int Mesh::_set(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");

	Point p = Point::pop(l);
	int i = luaL_checkinteger(l, 2);

	Point * source = (Point*)(m->vertexArray + m->dim*(i-1));

	source->x = p.x;
	source->y = p.y;

	return 0;
}

// get the ith vertex
int Mesh::_get(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");
	int i = luaL_checkint(l, 2);
	Point *source = (Point*)(m->vertexArray + m->dim*(i-1));

	lua_pushnumber(l, source->x);
	lua_pushnumber(l, source->y);
	if (m->dim > 3) lua_pushnumber(l, source->z);

	return m->dim;
}


int Mesh::_clone(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");
	Mesh *clone = (Mesh*)lua_newuserdata(l, sizeof(Mesh));

	luaL_getmetatable(l, "Mesh");
	lua_setmetatable(l, -2);

	*clone = *m; // copy data

	// copy the vertices
	clone->vertexArray = new double[clone->count*clone->dim];
	memcpy(clone->vertexArray, m->vertexArray, sizeof(double)*clone->count*clone->dim);
	
	if (m->normalArray) {
		clone->normalArray = new double[clone->count*clone->dim];
		memcpy(clone->normalArray, m->normalArray, sizeof(double)*clone->count*clone->dim);
	}

	return 1;
}


// get number of vertices
int Mesh::_count(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");
	lua_pushnumber(l, m->count);
	return 1;
}

// get dimension of vertices
int Mesh::_dim(lua_State *l) {
	Mesh *m = (Mesh*)luaL_checkudata(l, 1, "Mesh");
	lua_pushnumber(l, m->dim);
	return 1;
}



