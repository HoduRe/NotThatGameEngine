#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#define MAX_VERTICES 1000

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
	Primitives(PrimitiveEnum _type, float vertices[], int size);
	~Primitives();

	int SetVertices(float _vertices[], int size);	// Returns size
	float* GetVertices();	// Returns vertex vector

	PrimitiveEnum type;
	int size;
	int id;
	
private:
	float vertices[MAX_VERTICES];

};

class Cube : public Primitives {

public:

	Cube();
	Cube(float vertices[], int size);
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
