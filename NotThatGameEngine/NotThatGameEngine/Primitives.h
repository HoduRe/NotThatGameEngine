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

	int SetVertexVector(std::vector<float> _vertices);	// Returns size
	std::vector<float> GetVertexVector();	// Returns vertex vector

	int SetIndexVector(std::vector<unsigned int> _vertices);	// Returns size
	std::vector<unsigned int> GetIndexVector();	// Returns vertex vector

	bool BlitPrimitive();

	PrimitiveEnum type;
	int sizeVertexVector;
	int sizeIndexVector;
	int idVertex;
	int idIndex;
	
protected:
	std::vector<float> vertices;
	std::vector<unsigned int> index;

};

class CubeF : public PrimitivesF {

public:

	CubeF();
	CubeF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	~CubeF();

private:


};

class SphereF : public PrimitivesF {

public:

	SphereF();
	SphereF(std::vector<float> _vertices, std::vector<unsigned int> _index);
	SphereF(float radius, int stacks, int sectors, float scale);
	~SphereF();

	void SetAttributes(float radius, int stacks, int sectors, float scale);

private:
	
	void CreateVectors(float radius, int stacks, int sectors, float scale);

	float radius;
	int sectors;
	int stacks;
	float scale;

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
