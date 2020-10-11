#include "Primitives.h"

PrimitivesF::PrimitivesF(PrimitiveEnum _type) :
	type(_type), sizeVertexVector(0), sizeIndexVector(0), sizeNormalVector(0), idVertex(0), idIndex(0), idNormal(0), vertices(), indices(), normals() {}


PrimitivesF::PrimitivesF(PrimitiveEnum _type, std::vector<float> _vertices, std::vector<unsigned int> _indexBuffer) :
	type(_type), sizeNormalVector(0), idVertex(0), idIndex(0), idNormal(0), vertices(), indices(), normals() {

	sizeVertexVector = _vertices.size();
	sizeIndexVector = _indexBuffer.size();
	vertices = _vertices;
	indices = _indexBuffer;

	GLVertexBuffer();
	GLIndexBuffer();
}

PrimitivesF::~PrimitivesF() {}


int PrimitivesF::SetVertexVector(std::vector<float> _vertices) {

	sizeVertexVector = _vertices.size();
	vertices = _vertices;

	GLVertexBuffer();

	return sizeVertexVector;
}


std::vector<float> PrimitivesF::GetVertexVector() {	return vertices; }


int PrimitivesF::SetIndexVector(std::vector<unsigned int> _indexBuffer) {

	sizeIndexVector = _indexBuffer.size();
	indices = _indexBuffer;

	GLIndexBuffer();

	return sizeIndexVector;
}


std::vector<unsigned int> PrimitivesF::GetIndexVector() { return indices; }


int PrimitivesF::SetNormalVector(std::vector<float> _normalBuffer) {
	sizeNormalVector = _normalBuffer.size();
	normals = _normalBuffer;

	GLNormalBuffer();

	return sizeNormalVector;
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
	glGenBuffers(1, (GLuint*)&idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeVertexVector, GetVertexVector().data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void PrimitivesF::GLIndexBuffer() {
	glGenBuffers(1, (GLuint*)&idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sizeIndexVector, GetIndexVector().data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void PrimitivesF::GLNormalBuffer() {
	glGenBuffers(1, (GLuint*)&idNormal);
	glBindBuffer(GL_NORMAL_ARRAY, idNormal);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * sizeNormalVector, normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
}


// CUBE-------------------------------------------------------------------

CubeF::CubeF() : PrimitivesF(PrimitiveEnum::PRIMITIVE_CUBE) {}


CubeF::CubeF(std::vector<float> _vertices, std::vector<unsigned int> _indexBuffer) : PrimitivesF(PrimitiveEnum::PRIMITIVE_CUBE, _vertices, _indexBuffer) {}


CubeF::~CubeF() {}


// SPHERE-------------------------------------------------------------------

SphereF::SphereF() : PrimitivesF(PrimitiveEnum::PRIMITIVE_SPHERE), radius(0), stacks(0), sectors(0), scale(0.0f) {}


SphereF::SphereF(std::vector<float> _vertices, std::vector<unsigned int> _index) : PrimitivesF(PrimitiveEnum::PRIMITIVE_SPHERE, _vertices, _index), radius(0), stacks(0), sectors(0), scale(0.0f) {}


SphereF::SphereF(float _radius, int _stacks, int _sectors, float _scale) : PrimitivesF(PrimitiveEnum::PRIMITIVE_SPHERE), radius(_radius), stacks(_stacks), sectors(_sectors), scale(_scale) {
	CreateVertices(_radius, _stacks, _sectors, _scale);
}


SphereF::~SphereF() {}


void SphereF::SetAttributes(float _radius, int _stacks, int _sectors, float _scale) {
	radius = _radius;
	stacks = _stacks;
	sectors = _sectors;
	scale = _scale;
	if (vertices.size() == 0 && indices.size() == 0) { CreateVertices(radius, stacks, sectors, scale); }
}


void SphereF::CreateVertices(float _radius, int _stacks, int _sectors, float _scale) {

	std::vector<int> points;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * 3.1416 / _sectors;
	float stackStep = 3.1416 / _stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= _stacks; ++i)
	{
		stackAngle = 3.1416 / 2 - i * stackStep;    // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= _sectors; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			points.push_back(x);
			points.push_back(y);
			points.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);
		}
	}


	float k1, k2;
	for (int i = 0; i < _stacks; ++i)
	{
		k1 = i * (_sectors + 1);     // beginning of current stack
		k2 = k1 + _sectors + 1;      // beginning of next stack

		for (int j = 0; j < _sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				vertices.push_back(k1);
				vertices.push_back(k2);
				vertices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (_stacks - 1))
			{
				vertices.push_back(k1 + 1);
				vertices.push_back(k2);
				vertices.push_back(k2 + 1);
			}
		}
	}

	int size = vertices.size();
	sizeVertexVector = size;

	sizeNormalVector = normals.size();

	GLVertexBuffer();
	GLNormalBuffer();
}

// PYRAMID-------------------------------------------------------------------


// CYLINDER-------------------------------------------------------------------


