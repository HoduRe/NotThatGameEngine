#include "Primitives.h"

Primitives::Primitives(PrimitiveEnum _type, float* vertexs[]) : type(_type) {
	int i = 0;

	while (vertexs[i] != nullptr) { i++; }

	size = i;
}

Primitives::~Primitives() {

}


