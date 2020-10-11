#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include <vector>
#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"

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

	void SetNormalVector(std::vector<float> _normalBuffer);	// Returns size
	std::vector<float> GetNormalVector();	// Returns normal vector

	bool BlitPrimitive();

	void GLVertexBuffer();
	void GLIndexBuffer();
	void GLNormalBuffer();

	PrimitiveEnum type;
	int sizeVertexVector;
	int sizeIndexVector;
	int sizeNormalVector;
	int idVertex;
	int idIndex;
	int idNormal;
	
protected:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<float> normals;

};

class CubeF : public PrimitivesF {

public:

	CubeF();
	CubeF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	CubeF(float edgeLength, float scale);
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

	PyramidF(float* vertexs);
	~PyramidF();

private:


};

class CylinderF : public PrimitivesF {

public:

	CylinderF(float* vertexs);
	~CylinderF();

private:


};

#endif
