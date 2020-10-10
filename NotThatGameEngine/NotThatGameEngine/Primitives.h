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

	Primitives(PrimitiveEnum _type, std::vector<float>* vertices);
	~Primitives();

	PrimitiveEnum type;

private:

	float* vertices;
	int size;

};

class Cube : public Primitives {

public:

	Cube(std::vector<float>* vertices);
	~Cube();

private:


};

class Sphere : public Primitives {

public:

	Sphere(float* vertexs[]);
	~Sphere();

private:


};

class Pyramid : public Primitives {

public:

	Pyramid(float* vertexs[]);
	~Pyramid();

private:


};

class Cylinder : public Primitives {

public:

	Cylinder(float* vertexs[]);
	~Cylinder();

private:


};

#endif
