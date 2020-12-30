#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"

enum class PrimitiveEnum {
	PRIMITIVE_NONE,
	PRIMITIVE_CUBE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_PYRAMID,
	PRIMITIVE_CYLINDER
};

namespace PrimitivesF {

	void SetCubeVertices(float _edgeLength, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize);


	void SetSphereVertices(float radius, int stacks, int sectors, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize);


	void SetPyramidVertices(float _height, float _baseEdgeLength, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize);


	void SetCylinderVertices(float _height, float _radius, int _sectors, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize);

}

#endif
