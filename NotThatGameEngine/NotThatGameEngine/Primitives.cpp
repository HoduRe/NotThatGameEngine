#include "Primitives.h"
#include "OpenGLFuncionality.h"


void PrimitivesF::SetCubeVertices(float _edgeLength, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize) {

	*vertexSize = 24;
	*indexSize = 36;
	vertices = new float[*vertexSize];
	indices = new GLuint[*indexSize];
	

	vertices[0] = 0; vertices[1] = 0; vertices[2] = 0;
	vertices[3] = 0; vertices[4] = 0; vertices[5] = _edgeLength;
	vertices[6] = _edgeLength; vertices[7] = 0; vertices[8] = _edgeLength;
	vertices[9] = _edgeLength; vertices[10] = 0; vertices[11] = 0;

	vertices[12] = 0; vertices[13] = _edgeLength; vertices[14] = 0;
	vertices[15] = 0; vertices[16] = _edgeLength; vertices[17] = _edgeLength;
	vertices[18] = _edgeLength; vertices[19] = _edgeLength; vertices[20] = _edgeLength;
	vertices[21] = _edgeLength; vertices[22] = _edgeLength; vertices[23] = 0;

	indices[0] = 0;  indices[1] = 2;	indices[2] = 1;		indices[3] = 0;  indices[4] = 3;	indices[5] = 2;		// Up
	indices[6] = 5;  indices[7] = 7;	indices[8] = 4;		indices[9] = 5;	 indices[10] = 6;	indices[11] = 7;	// Down
	indices[12] = 7; indices[13] = 3;	indices[14] = 0;	indices[15] = 7; indices[16] = 0;	indices[17] = 4;	// Front
	indices[18] = 6; indices[19] = 1;	indices[20] = 2;	indices[21] = 6; indices[22] = 5;	indices[23] = 1;	// Back
	indices[24] = 6; indices[25] = 2;	indices[26] = 3;	indices[27] = 6; indices[28] = 3;	indices[29] = 7;	// Right
	indices[30] = 4; indices[31] = 0;	indices[32] = 1;	indices[33] = 4; indices[34] = 1;	indices[35] = 5;	// Left

}


void PrimitivesF::SetSphereVertices(float radius, int stacks, int sectors, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize) {

	std::vector<float> verticesVec;
	std::vector<GLuint> indicesVec;

	if (stacks < 3) { stacks = 3; }
	if (sectors < 3) { sectors = 3; }
	struct Vertex { float x, y, z; };
	std::vector<Vertex> tmpVertices;

	float x, y, z, xy;
	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = (PI / 2) - (i * stackStep);
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;

			Vertex vertex;
			vertex.x = xy * cosf(sectorAngle);
			vertex.y = xy * sinf(sectorAngle);
			vertex.z = z;
			tmpVertices.push_back(vertex);

		}

		unsigned int k1, k2;
		for (int i = 0; i < stacks; ++i)
		{
			k1 = i * (sectors + 1);
			k2 = k1 + sectors + 1;

			for (int j = 0; j < sectors; ++j, ++k1, ++k2)
			{

				if (i != 0)
				{
					indicesVec.push_back(k1);
					indicesVec.push_back(k2);
					indicesVec.push_back(k1 + 1);
				}

				if (i != (stacks - 1))
				{
					indicesVec.push_back(k1 + 1);
					indicesVec.push_back(k2);
					indicesVec.push_back(k2 + 1);
				}
			}
		}
	}

	Vertex v1, v2, v3, v4;
	std::vector<float> n;

	int i, j, k, vi1, vi2;
	int index = 0;
	for (i = 0; i < stacks; ++i)
	{
		vi1 = i * (sectors + 1);
		vi2 = (i + 1) * (sectors + 1);

		for (j = 0; j < sectors; ++j, ++vi1, ++vi2)
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

				indicesVec.push_back(index);
				indicesVec.push_back(index + 1);
				indicesVec.push_back(index + 2);

				index += 3;
			}
			else if (i == (stacks - 1)) // a triangle for last stack =========
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v3);

				indicesVec.push_back(index);
				indicesVec.push_back(index + 1);
				indicesVec.push_back(index + 2);

				index += 3;
			}
			else // 2 triangles for others ====================================
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v3);
				tmpVertices.push_back(v4);

				indicesVec.push_back(index);
				indicesVec.push_back(index + 1);
				indicesVec.push_back(index + 2);
				indicesVec.push_back(index + 2);
				indicesVec.push_back(index + 1);
				indicesVec.push_back(index + 3);

				index += 4;
			}
		}
	}

	for (int i = 0; i < tmpVertices.size(); i++) {
		verticesVec.push_back(tmpVertices[i].x);
		verticesVec.push_back(tmpVertices[i].y);
		verticesVec.push_back(tmpVertices[i].z);
	}

	*vertexSize = verticesVec.size();
	*indexSize = indicesVec.size();
	
	vertices = new float[*vertexSize];
	indices = new GLuint[*indexSize];

	for (int i = 0; i < verticesVec.size(); i++) { vertices[i] = verticesVec[i]; }
	for (int i = 0; i < indicesVec.size(); i++) { indices[i] = indicesVec[i]; }

}


void PrimitivesF::SetPyramidVertices(float _height, float _baseEdgeLength, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize) {

	*vertexSize = 15;
	*indexSize = 18;
	vertices = new float[*vertexSize];
	indices = new GLuint[*indexSize];


	vertices[0] = 0; vertices[1] = 0; vertices[2] = 0;
	vertices[3] = _baseEdgeLength; vertices[4] = 0; vertices[5] = 0;
	vertices[6] = _baseEdgeLength; vertices[7] = 0; vertices[8] = _baseEdgeLength;
	vertices[9] = 0; vertices[10] = 0; vertices[11] = _baseEdgeLength;
	vertices[12] = _baseEdgeLength * 0.5; vertices[13] = _height; vertices[14] = _baseEdgeLength * 0.5;


	indices[0] = 0; indices[1] = 4; indices[2] = 1; 	// Front side
	indices[3] = 1; indices[4] = 4; indices[5] = 2; 	// Right side
	indices[6] = 2; indices[7] = 4; indices[8] = 3; 	// Back side
	indices[9] = 3; indices[10] = 4; indices[11] = 0; 	// Left side
	indices[12] = 0; indices[13] = 2; indices[14] = 3;	indices[15] = 0; indices[16] = 1; indices[17] = 2;	// Base

}


void PrimitivesF::SetCylinderVertices(float _height, float _radius, int _sectors, float*& vertices, GLuint*& indices, int* vertexSize, int* indexSize) {

	if (_sectors < 3) { _sectors = 3; }

	// Create the inferior platform, including the central point (0.0f, 0.0f, 0.0f)
	float angleDiversion = 2 * PI / _sectors;
	float vertex2;
	float vertex3;
	float secondVertex2;
	float secondVertex3;
	std::vector<float> verticesVec;
	std::vector<GLuint> indicesVec;

	for (int i = 0; i < _sectors; i++) {

		if (i == 0) {
			vertex2 = _radius * cosf(i * angleDiversion);
			vertex3 = _radius * sinf(i * angleDiversion);
		}

		secondVertex2 = _radius * cosf((i + 1) * angleDiversion);
		secondVertex3 = _radius * sinf((i + 1) * angleDiversion);

		verticesVec.push_back(0.0f); verticesVec.push_back(0.0f); verticesVec.push_back(0.0f);
		verticesVec.push_back(vertex2); verticesVec.push_back(0.0f); verticesVec.push_back(vertex3);
		verticesVec.push_back(secondVertex2); verticesVec.push_back(0.0f); verticesVec.push_back(secondVertex3);

		vertex2 = secondVertex2;
		vertex3 = secondVertex3;

	}

	// Copy it but with +_height in Y axis and reverse order for reversed normals

	int firstCircleSize = verticesVec.size();

	for (int i = 0; i < _sectors; i++) {

		if (i == 0) {
			vertex2 = _radius * cosf(i * angleDiversion);
			vertex3 = _radius * sinf(i * angleDiversion);
		}

		secondVertex2 = _radius * cosf((i + 1) * angleDiversion);
		secondVertex3 = _radius * sinf((i + 1) * angleDiversion);

		verticesVec.push_back(0.0f); verticesVec.push_back(_height); verticesVec.push_back(0.0f);
		verticesVec.push_back(secondVertex2); verticesVec.push_back(_height); verticesVec.push_back(secondVertex3);
		verticesVec.push_back(vertex2); verticesVec.push_back(_height); verticesVec.push_back(vertex3);

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
			firstPointX = verticesVec.at(i);
			loopRegulator++;
			break;
		case 4:
			firstPointY = verticesVec.at(i);
			loopRegulator++;
			break;
		case 5:
			firstPointZ = verticesVec.at(i);
			loopRegulator++;
			break;
		case 6:
			secondPointX = verticesVec.at(i);
			loopRegulator++;
			break;
		case 7:
			secondPointY = verticesVec.at(i);
			loopRegulator++;
			break;
		case 8:
			secondPointZ = verticesVec.at(i);

			// First triangle
			verticesVec.push_back(firstPointX);
			verticesVec.push_back(firstPointY + _height);
			verticesVec.push_back(firstPointZ);
			verticesVec.push_back(secondPointX);
			verticesVec.push_back(secondPointY + _height);
			verticesVec.push_back(secondPointZ);
			verticesVec.push_back(firstPointX);
			verticesVec.push_back(firstPointY);
			verticesVec.push_back(firstPointZ);

			// Second triangle
			verticesVec.push_back(secondPointX);
			verticesVec.push_back(secondPointY + _height);
			verticesVec.push_back(secondPointZ);
			verticesVec.push_back(secondPointX);
			verticesVec.push_back(secondPointY);
			verticesVec.push_back(secondPointZ);
			verticesVec.push_back(firstPointX);
			verticesVec.push_back(firstPointY);
			verticesVec.push_back(firstPointZ);

			loopRegulator = 0;
			break;
		default:
			loopRegulator++;
			break;
		}

	}

	*vertexSize = verticesVec.size();

	for (GLuint i = 0; i < *vertexSize; i++) { indicesVec.push_back(i); }

	*indexSize = indicesVec.size();

	vertices = new float[*vertexSize];
	indices = new GLuint[*indexSize];

	for (int i = 0; i < *vertexSize; i++) { vertices[i] = verticesVec[i]; }
	for (int i = 0; i < *indexSize; i++) { indices[i] = indicesVec[i]; }

}

