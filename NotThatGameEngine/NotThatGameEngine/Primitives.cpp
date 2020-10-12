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


std::vector<float> PrimitivesF::GetVertexVector() { return vertices; }


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

		if (sizeNormalVector != 0) {
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


CubeF::CubeF(float _edgeLength) : PrimitivesF(PrimitiveEnum::PRIMITIVE_CUBE), edgeLength(_edgeLength) {
	SetAttributes(_edgeLength);
}


void CubeF::SetAttributes(float _edgeLength) {

	edgeLength = _edgeLength;

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

	if (stacks < 3) { stacks = 3; }
	if (sectors < 3) { sectors = 3; }

	CreateVertices(radius, stacks, sectors);
}


void SphereF::CreateVertices(float _radius, int _stacks, int _sectors) {



	GLVertexBuffer();
	GLNormalBuffer();
}

// PYRAMID-------------------------------------------------------------------
PyramidF::PyramidF() : PrimitivesF(PrimitiveEnum::PRIMITIVE_PYRAMID), height(0), baseEdgeLength(0) {}


PyramidF::PyramidF(std::vector<float> _vertices, std::vector<unsigned int> _index) : PrimitivesF(PrimitiveEnum::PRIMITIVE_PYRAMID, _vertices, _index), height(0), baseEdgeLength(0) {}


PyramidF::PyramidF(float _height, float _baseEdgeLength) : PrimitivesF(PrimitiveEnum::PRIMITIVE_PYRAMID), height(0), baseEdgeLength(0) {
	SetAttributes(_height, _baseEdgeLength);
}


PyramidF::~PyramidF() {}


void PyramidF::SetAttributes(float _height, float _baseEdgeLength) {

	height = _height;
	baseEdgeLength = _baseEdgeLength;

	vertices.push_back(0); vertices.push_back(0); vertices.push_back(0);
	vertices.push_back(_baseEdgeLength); vertices.push_back(0); vertices.push_back(0);
	vertices.push_back(_baseEdgeLength); vertices.push_back(0); vertices.push_back(_baseEdgeLength);
	vertices.push_back(0); vertices.push_back(0); vertices.push_back(_baseEdgeLength);
	vertices.push_back(_baseEdgeLength * 0.5); vertices.push_back(_height); vertices.push_back(_baseEdgeLength * 0.5);


	indices.push_back(0); indices.push_back(1);	indices.push_back(4);	// Front side
	indices.push_back(1); indices.push_back(2);	indices.push_back(4);	// Right side
	indices.push_back(2); indices.push_back(3);	indices.push_back(4);	// Back side
	indices.push_back(3); indices.push_back(0);	indices.push_back(4);	// Left side
	indices.push_back(0); indices.push_back(3);	indices.push_back(2);	indices.push_back(0); indices.push_back(2);	indices.push_back(1);	// Base


	GLVertexBuffer();
	GLIndexBuffer();
}

// CYLINDER-------------------------------------------------------------------
CylinderF::CylinderF() : PrimitivesF(PrimitiveEnum::PRIMITIVE_CYLINDER), height(0), radius(0), sectors(0) {}


CylinderF::CylinderF(std::vector<float> _vertices, std::vector<unsigned int> _index) : PrimitivesF(PrimitiveEnum::PRIMITIVE_CYLINDER, _vertices, _index), height(0), radius(0), sectors(0) {}


CylinderF::CylinderF(float _height, float _radius, float _baseEdgeLength) : PrimitivesF(PrimitiveEnum::PRIMITIVE_CYLINDER), height(0), radius(0), sectors(0) {
	SetAttributes(_height, _radius, _baseEdgeLength);
}


CylinderF::~CylinderF() {}


void CylinderF::SetAttributes(float _height, float _radius, int _sectors) {

	height = _height;
	radius = _radius;
	sectors = _sectors;

	if (sectors < 3) { sectors = 3; }

	// Create the inferior platform, including the central point (0.0f, 0.0f, 0.0f)
	float angleDiversion = 2 * 3.14159265359 / sectors;
	float vertex2;
	float vertex3;
	float secondVertex2;
	float secondVertex3;

	for (int i = 0; i < sectors; i++) {

		if (i == 0) {
			vertex2 = radius * cosf(i * angleDiversion);
			vertex3 = radius * sinf(i * angleDiversion);
		}

		secondVertex2 = radius * cosf((i + 1) * angleDiversion);
		secondVertex3 = radius * sinf((i + 1) * angleDiversion);

		vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
		vertices.push_back(vertex2); vertices.push_back(0.0f); vertices.push_back(vertex3);
		vertices.push_back(secondVertex2); vertices.push_back(0.0f); vertices.push_back(secondVertex3);

		vertex2 = secondVertex2;
		vertex3 = secondVertex3;

	}

	// Copy it but with +_height in Y axis and reverse order for reversed normals

	int firstCircleSize = vertices.size();

	for (int i = 0; i < sectors; i++) {

		if (i == 0) {
			vertex2 = radius * cosf(i * angleDiversion);
			vertex3 = radius * sinf(i * angleDiversion);
		}

		secondVertex2 = radius * cosf((i + 1) * angleDiversion);
		secondVertex3 = radius * sinf((i + 1) * angleDiversion);

		vertices.push_back(0.0f); vertices.push_back(height); vertices.push_back(0.0f);
		vertices.push_back(secondVertex2); vertices.push_back(height); vertices.push_back(secondVertex3);
		vertices.push_back(vertex2); vertices.push_back(height); vertices.push_back(vertex3);
		
		vertex2 = secondVertex2;
		vertex3 = secondVertex3;

	}

	// Make triangles between both circles

	float firstPointX;
	float firstPointY;
	float firstPointZ;
	
	float secondPointX;
	float secondPointY;
	float secondPointZ;

	int loopRegulator = 0;

	for (int i = 0; i < firstCircleSize; i++) {

		switch (loopRegulator) {
		case 3:
			firstPointX = vertices[i];
			loopRegulator++;
			break;
		case 4:
			firstPointY = vertices[i];
			loopRegulator++;
			break;
		case 5:
			firstPointZ = vertices[i];
			loopRegulator++;
			break;
		case 6:
			secondPointX = vertices[i];
			loopRegulator++;
			break;
		case 7:
			secondPointY = vertices[i];
			loopRegulator++;
			break;
		case 8:
			secondPointZ = vertices[i];

			// First triangle
			vertices.push_back(firstPointX);
			vertices.push_back(firstPointY + height);
			vertices.push_back(firstPointZ);
			vertices.push_back(secondPointX);
			vertices.push_back(secondPointY + height);
			vertices.push_back(secondPointZ);
			vertices.push_back(firstPointX);
			vertices.push_back(firstPointY);
			vertices.push_back(firstPointZ);

			// Second triangle
			vertices.push_back(secondPointX);
			vertices.push_back(secondPointY + height);
			vertices.push_back(secondPointZ);
			vertices.push_back(secondPointX);
			vertices.push_back(secondPointY);
			vertices.push_back(secondPointZ);
			vertices.push_back(firstPointX);
			vertices.push_back(firstPointY);
			vertices.push_back(firstPointZ);

			loopRegulator = 0;
			break;
		default:
			loopRegulator++;
			break;
		}

	}


	GLVertexBuffer();
}

