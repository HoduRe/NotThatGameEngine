#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "OpenGLFuncionality.h"

enum class PrimitiveEnum {
	PRIMITIVE_NONE,
	PRIMITIVE_CUBE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_PYRAMID,
	PRIMITIVE_CYLINDER
};


void SetCubeVertices(float _edgeLength, std::vector<float>* vertices, std::vector<GLuint>* indices);


void SetSphereVertices(float radius, int stacks, int sectors, std::vector<float>* vertices, std::vector<GLuint>* indices);


void SetPyramidVertices(float _height, float _baseEdgeLength, std::vector<float>* vertices, std::vector<GLuint>* indices);


void SetCylinderVertices(float _height, float _radius, int _sectors, std::vector<float>* vertices, std::vector<GLuint>* indices);

#endif
