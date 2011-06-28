
#ifndef MESH_H_
#define MESH_H_

#include "geometry.h"

namespace aroma {

void register_Mesh(lua_State *l);

/**
 * a 2d or 3d mesh to be rendered as vertex array
 */
class Mesh {
	int dim, count;
	double *vertexArray;
	double *normalArray;
	GLenum format;
public:
	static int _new(lua_State *l);

	static int _render(lua_State *l);
	static int _setNormals(lua_State *l);
	static int _dump(lua_State *l);

	static int _vertices(lua_State *l); // vertex iterator
	static int _verticesIter(lua_State *l);


	static int _count(lua_State *l);
	static int _dim(lua_State *l);

	static int _set(lua_State *l);
	static int _get(lua_State *l); // get a vertex

	static int _clone(lua_State *l);
	static int _gc(lua_State *l); // garbage collect
};

}

#endif /* MESH_H_ */
