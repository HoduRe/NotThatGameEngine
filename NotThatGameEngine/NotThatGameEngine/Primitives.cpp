#include "Primitives.h"

Primitives::Primitives(PrimitiveEnum _type) : type(_type), sizeVertexVector(0), sizeIndexVector(0), idVertex(0), idIndex(0), vertices(), index() {}


Primitives::Primitives(PrimitiveEnum _type, std::vector<float> _vertices, std::vector<int> _indexBuffer) : type(_type), idVertex(0), idIndex(0), vertices(), index() {

	sizeVertexVector = _vertices.size();
	sizeIndexVector = _indexBuffer.size();
	vertices = _vertices;
	index = _indexBuffer;

}

Primitives::~Primitives() {}


int Primitives::SetVertexVector(std::vector<float> _vertices) {

	sizeVertexVector = _vertices.size();
	vertices = _vertices;

	return sizeVertexVector;
}


std::vector<float> Primitives::GetVertexVector() { return vertices; }


int Primitives::SetIndexVector(std::vector<int> _indexBuffer) {

	sizeIndexVector = _indexBuffer.size();
	index = _indexBuffer;

	return sizeIndexVector;
}


std::vector<int> Primitives::GetIndexVector() { return index; }


Cube::Cube() : Primitives(PrimitiveEnum::PRIMITIVE_CUBE){}


Cube::Cube(std::vector<float> _vertices, std::vector<int> _indexBuffer) : Primitives (PrimitiveEnum::PRIMITIVE_CUBE, _vertices, _indexBuffer){}


Cube::~Cube(){}

