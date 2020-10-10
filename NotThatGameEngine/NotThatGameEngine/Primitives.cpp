#include "Primitives.h"

Primitives::Primitives(PrimitiveEnum _type) : type(_type), size(0), vertices(), id(0){}


Primitives::Primitives(PrimitiveEnum _type, float _vertices[], int _size) : type(_type), id(0) {

	size = _size;
	for (int i = 0; i < size; i++) {
		vertices[i] = _vertices[i];
	}
}

Primitives::~Primitives() {}


int Primitives::SetVertices(float _vertices[], int _size) {

	size = _size;
	for (int i = 0; i < size; i++) {
		vertices[i] = _vertices[i];
	}

	return size;
}


float* Primitives::GetVertices() { return vertices; }


Cube::Cube() : Primitives(PrimitiveEnum::PRIMITIVE_CUBE){}


Cube::Cube(float _vertices[], int _size) : Primitives (PrimitiveEnum::PRIMITIVE_CUBE, _vertices, _size){}


Cube::~Cube(){}

