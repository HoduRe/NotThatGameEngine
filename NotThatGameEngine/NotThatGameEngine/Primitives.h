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

class Primitives {

public:

	Primitives(PrimitiveEnum _type);
	Primitives(PrimitiveEnum _type, std::vector<float> _vertices, std::vector<unsigned int> _index);
	~Primitives();

	int SetVertexVector(std::vector<float> _vertices);	// Returns size
	std::vector<float> GetVertexVector();	// Returns vertex vector

	int SetIndexVector(std::vector<unsigned int> _vertices);	// Returns size
	std::vector<unsigned int> GetIndexVector();	// Returns vertex vector

	PrimitiveEnum type;
	int sizeVertexVector;
	int sizeIndexVector;
	int idVertex;
	int idIndex;
	
private:
	std::vector<float> vertices;
	std::vector<unsigned int> index;

};

class Cube : public Primitives {

public:

	Cube();
	Cube(std::vector<float> _vertices, std::vector<unsigned int> _index);
	~Cube();

private:


};

class Sphere : public Primitives {

public:

	Sphere(float vertexs);
	~Sphere();

private:


};

class Pyramid : public Primitives {

public:

	Pyramid(float* vertexs);
	~Pyramid();

private:


};

class Cylinder : public Primitives {

public:

	Cylinder(float* vertexs);
	~Cylinder();

private:


};

#endif
