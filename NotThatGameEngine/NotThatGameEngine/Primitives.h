#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

enum class PrimitiveEnum {
	PRIMITIVE_NONE,
	PRIMITIVE_CUBE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_PYRAMID,
	PRIMITIVE_CYLINDER
};

class Primitives {

public:
	Primitives(PrimitiveEnum _type, float* vertexs[]);
	~Primitives();

	PrimitiveEnum type;

private:

	int size;

};

class Cube : public Primitives {

public:

private:


};

class Sphere : public Primitives {

public:

private:


};

class Pyramid : public Primitives {

public:

private:


};

class Cylinder : public Primitives {

public:

private:


};

#endif
