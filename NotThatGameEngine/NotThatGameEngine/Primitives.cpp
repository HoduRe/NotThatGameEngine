#include "Primitives.h"

Primitives::Primitives(PrimitiveEnum _type, std::vector<float>* _vertices) : type(_type) {
	size = _vertices->size();

	float* arrayVertex = new float[size];
	int i;
	for (i = 0; i < size; i++) {
		arrayVertex[i] = i+1;
	}

	/*for (i = 0; i < size; i++) {
		 
		LOG("%d", arrayVertex[i]);
	}*/

	vertices = arrayVertex;
}

Primitives::~Primitives() {}


Cube::Cube(std::vector<float>* vertices) : Primitives (PrimitiveEnum::PRIMITIVE_CUBE, vertices){
}


Cube::~Cube(){}

