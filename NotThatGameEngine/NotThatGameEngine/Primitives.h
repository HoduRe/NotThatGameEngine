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

class PrimitivesF {

public:

	PrimitivesF(PrimitiveEnum _type);
	PrimitivesF(PrimitiveEnum _type, std::vector<float> _vertices, std::vector<unsigned int> _index);
	~PrimitivesF();

	void SetVertexVector(std::vector<float> _vertices);	// Returns size
	std::vector<float> GetVertexVector();	// Returns vertex vector

	void SetIndexVector(std::vector<unsigned int> _indexBuffer);	// Returns size
	std::vector<unsigned int> GetIndexVector();	// Returns index vector

	bool BlitPrimitive();

	PrimitiveEnum type;

	GLuint idVertex;
	GLuint idIndex;
	
protected:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

};

class CubeF : public PrimitivesF {

public:

	CubeF();
	CubeF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	CubeF(float edgeLength);
	~CubeF();

	void SetAttributes(float _edgeLength);

private:

	float edgeLength;
};

class SphereF : public PrimitivesF {

public:

	SphereF();
	SphereF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	SphereF(float radius, int stacks, int sectors);
	~SphereF();

	void SetAttributes(float radius, int stacks, int sectors);

private:
	
	void CreateVertices(float radius, int stacks, int sectors);

	float radius;
	int sectors;
	int stacks;

};

class PyramidF : public PrimitivesF {

public:

	PyramidF();
	PyramidF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	PyramidF(float _height, float _baseEdgeLength);
	~PyramidF();

	void SetAttributes(float _height, float _baseEdgeLength);

private:

	float height;
	float baseEdgeLength;

};

class CylinderF : public PrimitivesF {

public:

	CylinderF();
	CylinderF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	CylinderF(float _height, float _radius, float _baseEdgeLength);
	~CylinderF();

	void SetAttributes(float _height, float _radius, int _sectors);

private:

	float height;
	float radius;
	int sectors;

};

#endif
