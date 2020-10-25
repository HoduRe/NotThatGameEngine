#include "Primitives.h"

PrimitivesF::PrimitivesF(PrimitiveEnum _type) : type(_type), idVertex(0), idIndex(0), vertices(), indices() {}


PrimitivesF::PrimitivesF(PrimitiveEnum _type, std::vector<float> _vertices, std::vector<unsigned int> _indexBuffer) :
	type(_type), idVertex(0), idIndex(0), vertices(), indices() {

	vertices = _vertices;
	indices = _indexBuffer;

	LoadDataBufferFloat(GL_ARRAY_BUFFER, &idVertex, vertices.size(), vertices.data());
	LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &idIndex, indices.size(), indices.data());
}

PrimitivesF::~PrimitivesF() {}


void PrimitivesF::SetVertexVector(std::vector<float> _vertices) {

	vertices = _vertices;
	LoadDataBufferFloat(GL_ARRAY_BUFFER, &idVertex, vertices.size(), vertices.data());
}


std::vector<float> PrimitivesF::GetVertexVector() { return vertices; }


void PrimitivesF::SetIndexVector(std::vector<unsigned int> _indexBuffer) {

	indices = _indexBuffer;
	LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &idIndex, indices.size(), indices.data());
}


std::vector<unsigned int> PrimitivesF::GetIndexVector() { return indices; }


bool PrimitivesF::BlitPrimitive() {

	bool ret = true;

	if (vertices.size() == 0) { ret = false; }
	else if (indices.size() == 0) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	return ret;
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


	LoadDataBufferFloat(GL_ARRAY_BUFFER, &idVertex, vertices.size(), vertices.data());
	LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &idIndex, indices.size(), indices.data());
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

	struct Vertex { float x, y, z; };
	std::vector<Vertex> tmpVertices;

	float x, y, z, xy;
	float sectorStep = 2 * PI / _sectors;
	float stackStep = PI / _stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= _stacks; ++i)
	{
		stackAngle = (PI / 2) - (i * stackStep);
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (int j = 0; j <= _sectors; ++j)
		{
			sectorAngle = j * sectorStep;

			Vertex vertex;
			vertex.x = xy * cosf(sectorAngle);
			vertex.y = xy * sinf(sectorAngle);
			vertex.z = z;
			tmpVertices.push_back(vertex);

		}

		unsigned int k1, k2;
		for (int i = 0; i < _stacks; ++i)
		{
			k1 = i * (_sectors + 1);
			k2 = k1 + _sectors + 1;

			for (int j = 0; j < _sectors; ++j, ++k1, ++k2)
			{

				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (_stacks - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
	}

	Vertex v1, v2, v3, v4;
	std::vector<float> n;

	int i, j, k, vi1, vi2;
	int index = 0;
	for (i = 0; i < _stacks; ++i)
	{
		vi1 = i * (_sectors + 1);
		vi2 = (i + 1) * (_sectors + 1);

		for (j = 0; j < _sectors; ++j, ++vi1, ++vi2)
		{
			//  v1--v3
			//  |    |
			//  v2--v4
			v1 = tmpVertices[vi1];
			v2 = tmpVertices[vi2];
			v3 = tmpVertices[vi1 + 1];
			v4 = tmpVertices[vi2 + 1];

			if (i == 0) // a triangle for first stack ==========================
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v4);

				indices.push_back(index);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				index += 3;
			}
			else if (i == (_stacks - 1)) // a triangle for last stack =========
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v3);

				indices.push_back(index);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				index += 3;
			}
			else // 2 triangles for others ====================================
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v3);
				tmpVertices.push_back(v4);

				indices.push_back(index);
				indices.push_back(index + 1);
				indices.push_back(index + 2);
				indices.push_back(index + 2);
				indices.push_back(index + 1);
				indices.push_back(index + 3);

				index += 4;
			}
		}
	}

	for (int i = 0; i < tmpVertices.size(); i++) {
		vertices.push_back(tmpVertices[i].x);
		vertices.push_back(tmpVertices[i].y);
		vertices.push_back(tmpVertices[i].z);
	}

	LoadDataBufferFloat(GL_ARRAY_BUFFER, &idVertex, vertices.size(), vertices.data());
	LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &idIndex, indices.size(), indices.data());
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


	LoadDataBufferFloat(GL_ARRAY_BUFFER, &idVertex, vertices.size(), vertices.data());
	LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &idIndex, indices.size(), indices.data());
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
	float angleDiversion = 2 * PI / sectors;
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


	LoadDataBufferFloat(GL_ARRAY_BUFFER, &idVertex, vertices.size(), vertices.data());
}

