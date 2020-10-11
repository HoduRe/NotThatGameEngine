#include "Primitives.h"

PrimitivesF::PrimitivesF(PrimitiveEnum _type) :
	type(_type), sizeVertexVector(0), sizeIndexVector(0), sizeNormalVector(0), idVertex(0), idIndex(0), idNormal(0), vertices(), indices(), normals() {}


PrimitivesF::PrimitivesF(PrimitiveEnum _type, std::vector<float> _vertices, std::vector<unsigned int> _indexBuffer) :
	type(_type), sizeNormalVector(0), idVertex(0), idIndex(0), idNormal(0), vertices(), indices(), normals() {

	vertices = _vertices;
	indices = _indexBuffer;

	GLVertexBuffer();
	GLIndexBuffer();
}

PrimitivesF::~PrimitivesF() {}


void PrimitivesF::SetVertexVector(std::vector<float> _vertices) {

	vertices = _vertices;
	GLVertexBuffer();
}


std::vector<float> PrimitivesF::GetVertexVector() {	return vertices; }


void PrimitivesF::SetIndexVector(std::vector<unsigned int> _indexBuffer) {

	indices = _indexBuffer;
	GLIndexBuffer();
}


std::vector<unsigned int> PrimitivesF::GetIndexVector() { return indices; }


void PrimitivesF::SetNormalVector(std::vector<float> _normalBuffer) {

	normals = _normalBuffer;
	GLNormalBuffer();
}


std::vector<float> PrimitivesF::GetNormalVector() { return normals; }


bool PrimitivesF::BlitPrimitive() {
	bool ret = true;

	if (sizeVertexVector == 0) { ret = false; }
	else if (sizeIndexVector == 0) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		if(sizeNormalVector != 0){
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_NORMAL_ARRAY, idNormal);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}

		glDrawArrays(GL_TRIANGLES, 0, sizeVertexVector);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_NORMAL_ARRAY, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);

		if (sizeNormalVector != 0) {
			glEnableClientState(GL_NORMAL_ARRAY); 
			glBindBuffer(GL_NORMAL_ARRAY, idNormal);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}

		glDrawElements(GL_TRIANGLES, sizeIndexVector, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	return ret;
}


void PrimitivesF::GLVertexBuffer() {

	sizeVertexVector = vertices.size();

	glGenBuffers(1, (GLuint*)&idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeVertexVector, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void PrimitivesF::GLIndexBuffer() {

	sizeIndexVector = indices.size();

	glGenBuffers(1, (GLuint*)&idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sizeIndexVector, indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void PrimitivesF::GLNormalBuffer() {

	sizeNormalVector = normals.size();

	glGenBuffers(1, (GLuint*)&idNormal);
	glBindBuffer(GL_NORMAL_ARRAY, idNormal);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * sizeNormalVector, normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
}


// CUBE-------------------------------------------------------------------

CubeF::CubeF() : PrimitivesF(PrimitiveEnum::PRIMITIVE_CUBE), edgeLength(0) {}


CubeF::CubeF(std::vector<float> _vertices, std::vector<unsigned int> _indexBuffer) : PrimitivesF(PrimitiveEnum::PRIMITIVE_CUBE, _vertices, _indexBuffer), edgeLength(0) {}


CubeF::CubeF(float _edgeLength, float _scale) : PrimitivesF(PrimitiveEnum::PRIMITIVE_CUBE), edgeLength(_edgeLength){
	SetAttributes(_edgeLength);
}


void CubeF::SetAttributes(float _edgeLength) {

	vertices.push_back(0); vertices.push_back(0); vertices.push_back(0);
	vertices.push_back(0); vertices.push_back(0); vertices.push_back(_edgeLength);
	vertices.push_back(_edgeLength); vertices.push_back(0); vertices.push_back(_edgeLength);
	vertices.push_back(_edgeLength); vertices.push_back(0); vertices.push_back(0);

	vertices.push_back(0); vertices.push_back(_edgeLength); vertices.push_back(0);
	vertices.push_back(0); vertices.push_back(_edgeLength); vertices.push_back(_edgeLength);
	vertices.push_back(_edgeLength); vertices.push_back(_edgeLength); vertices.push_back(_edgeLength);
	vertices.push_back(_edgeLength); vertices.push_back(_edgeLength); vertices.push_back(0);

	indices.push_back(0); indices.push_back(2);	indices.push_back(1);	indices.push_back(0); indices.push_back(3);	indices.push_back(2);	// Up
	indices.push_back(5); indices.push_back(7);	indices.push_back(4);	indices.push_back(5); indices.push_back(6);	indices.push_back(7);	// Down
	indices.push_back(7); indices.push_back(3);	indices.push_back(0);	indices.push_back(7); indices.push_back(0);	indices.push_back(4);	// Front
	indices.push_back(6); indices.push_back(1);	indices.push_back(2);	indices.push_back(6); indices.push_back(5);	indices.push_back(1);	// Back
	indices.push_back(6); indices.push_back(2);	indices.push_back(3);	indices.push_back(6); indices.push_back(3);	indices.push_back(7);	// Right
	indices.push_back(4); indices.push_back(0);	indices.push_back(1);	indices.push_back(4); indices.push_back(1);	indices.push_back(5);	// Left


	GLVertexBuffer();
	GLIndexBuffer();
}


CubeF::~CubeF() {}


// SPHERE-------------------------------------------------------------------

SphereF::SphereF() : PrimitivesF(PrimitiveEnum::PRIMITIVE_SPHERE), radius(0), stacks(0), sectors(0) {}


SphereF::SphereF(std::vector<float> _vertices, std::vector<unsigned int> _index) : PrimitivesF(PrimitiveEnum::PRIMITIVE_SPHERE, _vertices, _index), radius(0), stacks(0), sectors(0) {}


SphereF::SphereF(float _radius, int _stacks, int _sectors) : PrimitivesF(PrimitiveEnum::PRIMITIVE_SPHERE), radius(_radius), stacks(_stacks), sectors(_sectors) {
	CreateVertices(_radius, _stacks, _sectors);
}


SphereF::~SphereF() {}


void SphereF::SetAttributes(float _radius, int _stacks, int _sectors) {
	radius = _radius;
	stacks = _stacks;
	sectors = _sectors;
	CreateVertices(radius, stacks, sectors);
}


void SphereF::CreateVertices(float _radius, int _stacks, int _sectors) {

	GLVertexBuffer();
	GLNormalBuffer();
}

// PYRAMID-------------------------------------------------------------------


// CYLINDER-------------------------------------------------------------------


