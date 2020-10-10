#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include <vector>

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
	Primitives(PrimitiveEnum _type, std::vector<float> _vertices);
	~Primitives();

	int SetVertices(std::vector<float> _vertices);	// Returns size
	std::vector<float>* GetVertices();	// Returns vertex vector

	PrimitiveEnum type;
	int size;
	int id;
	
private:
	std::vector<float> vertices;

};

class Cube : public Primitives {

public:

	Cube();
	Cube(std::vector<float> _vertices);
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
