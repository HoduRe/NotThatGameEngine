#include "Primitives.h"

Primitives::Primitives(PrimitiveEnum _type, std::vector<float>* _vertices) : type(_type) {
	size = _vertices->size();
	vertices = new float[size];

	for (int i = 0; i < size; i++) {
		vertices = _vertices->data();
	}

}

Primitives::~Primitives() {}


Cube::Cube(std::vector<float>* vertices) : Primitives (PrimitiveEnum::PRIMITIVE_CUBE, vertices){
}


Cube::~Cube(){}

