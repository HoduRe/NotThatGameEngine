#include "Primitives.h"


void SetCubeVertices(float _edgeLength, std::vector<float>* vertices, std::vector<GLuint>* indices) {

	vertices->push_back(0); vertices->push_back(0); vertices->push_back(0);
	vertices->push_back(0); vertices->push_back(0); vertices->push_back(_edgeLength);
	vertices->push_back(_edgeLength); vertices->push_back(0); vertices->push_back(_edgeLength);
	vertices->push_back(_edgeLength); vertices->push_back(0); vertices->push_back(0);

	vertices->push_back(0); vertices->push_back(_edgeLength); vertices->push_back(0);
	vertices->push_back(0); vertices->push_back(_edgeLength); vertices->push_back(_edgeLength);
	vertices->push_back(_edgeLength); vertices->push_back(_edgeLength); vertices->push_back(_edgeLength);
	vertices->push_back(_edgeLength); vertices->push_back(_edgeLength); vertices->push_back(0);

	indices->push_back(0); indices->push_back(2);	indices->push_back(1);	indices->push_back(0); indices->push_back(3);	indices->push_back(2);	// Up
	indices->push_back(5); indices->push_back(7);	indices->push_back(4);	indices->push_back(5); indices->push_back(6);	indices->push_back(7);	// Down
	indices->push_back(7); indices->push_back(3);	indices->push_back(0);	indices->push_back(7); indices->push_back(0);	indices->push_back(4);	// Front
	indices->push_back(6); indices->push_back(1);	indices->push_back(2);	indices->push_back(6); indices->push_back(5);	indices->push_back(1);	// Back
	indices->push_back(6); indices->push_back(2);	indices->push_back(3);	indices->push_back(6); indices->push_back(3);	indices->push_back(7);	// Right
	indices->push_back(4); indices->push_back(0);	indices->push_back(1);	indices->push_back(4); indices->push_back(1);	indices->push_back(5);	// Left

}


void SetSphereVertices(float radius, int stacks, int sectors, std::vector<float>* vertices, std::vector<GLuint>* indices) {

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
					indices->push_back(k1);
					indices->push_back(k2);
					indices->push_back(k1 + 1);
				}

				if (i != (stacks - 1))
				{
					indices->push_back(k1 + 1);
					indices->push_back(k2);
					indices->push_back(k2 + 1);
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

				indices->push_back(index);
				indices->push_back(index + 1);
				indices->push_back(index + 2);

				index += 3;
			}
			else if (i == (stacks - 1)) // a triangle for last stack =========
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v3);

				indices->push_back(index);
				indices->push_back(index + 1);
				indices->push_back(index + 2);

				index += 3;
			}
			else // 2 triangles for others ====================================
			{
				tmpVertices.push_back(v1);
				tmpVertices.push_back(v2);
				tmpVertices.push_back(v3);
				tmpVertices.push_back(v4);

				indices->push_back(index);
				indices->push_back(index + 1);
				indices->push_back(index + 2);
				indices->push_back(index + 2);
				indices->push_back(index + 1);
				indices->push_back(index + 3);

				index += 4;
			}
		}
	}

	for (int i = 0; i < tmpVertices.size(); i++) {
		vertices->push_back(tmpVertices[i].x);
		vertices->push_back(tmpVertices[i].y);
		vertices->push_back(tmpVertices[i].z);
	}

}


void SetPyramidVertices(float _height, float _baseEdgeLength, std::vector<float>* vertices, std::vector<GLuint>* indices) {

	vertices->push_back(0); vertices->push_back(0); vertices->push_back(0);
	vertices->push_back(_baseEdgeLength); vertices->push_back(0); vertices->push_back(0);
	vertices->push_back(_baseEdgeLength); vertices->push_back(0); vertices->push_back(_baseEdgeLength);
	vertices->push_back(0); vertices->push_back(0); vertices->push_back(_baseEdgeLength);
	vertices->push_back(_baseEdgeLength * 0.5); vertices->push_back(_height); vertices->push_back(_baseEdgeLength * 0.5);


	indices->push_back(0); indices->push_back(4); indices->push_back(1); 	// Front side
	indices->push_back(1); indices->push_back(4); indices->push_back(2); 	// Right side
	indices->push_back(2); indices->push_back(4); indices->push_back(3); 	// Back side
	indices->push_back(3); indices->push_back(4); indices->push_back(0); 	// Left side
	indices->push_back(0); indices->push_back(2); indices->push_back(3);	indices->push_back(0); indices->push_back(1); indices->push_back(2);	// Base

}


void SetCylinderVertices(float _height, float _radius, int _sectors, std::vector<float>* vertices, std::vector<GLuint>* indices) {

	if (_sectors < 3) { _sectors = 3; }

	// Create the inferior platform, including the central point (0.0f, 0.0f, 0.0f)
	float angleDiversion = 2 * PI / _sectors;
	float vertex2;
	float vertex3;
	float secondVertex2;
	float secondVertex3;

	for (int i = 0; i < _sectors; i++) {

		if (i == 0) {
			vertex2 = _radius * cosf(i * angleDiversion);
			vertex3 = _radius * sinf(i * angleDiversion);
		}

		secondVertex2 = _radius * cosf((i + 1) * angleDiversion);
		secondVertex3 = _radius * sinf((i + 1) * angleDiversion);

		vertices->push_back(0.0f); vertices->push_back(0.0f); vertices->push_back(0.0f);
		vertices->push_back(vertex2); vertices->push_back(0.0f); vertices->push_back(vertex3);
		vertices->push_back(secondVertex2); vertices->push_back(0.0f); vertices->push_back(secondVertex3);

		vertex2 = secondVertex2;
		vertex3 = secondVertex3;

	}

	// Copy it but with +_height in Y axis and reverse order for reversed normals

	int firstCircleSize = vertices->size();

	for (int i = 0; i < _sectors; i++) {

		if (i == 0) {
			vertex2 = _radius * cosf(i * angleDiversion);
			vertex3 = _radius * sinf(i * angleDiversion);
		}

		secondVertex2 = _radius * cosf((i + 1) * angleDiversion);
		secondVertex3 = _radius * sinf((i + 1) * angleDiversion);

		vertices->push_back(0.0f); vertices->push_back(_height); vertices->push_back(0.0f);
		vertices->push_back(secondVertex2); vertices->push_back(_height); vertices->push_back(secondVertex3);
		vertices->push_back(vertex2); vertices->push_back(_height); vertices->push_back(vertex3);

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
			firstPointX = vertices->at(i);
			loopRegulator++;
			break;
		case 4:
			firstPointY = vertices->at(i);
			loopRegulator++;
			break;
		case 5:
			firstPointZ = vertices->at(i);
			loopRegulator++;
			break;
		case 6:
			secondPointX = vertices->at(i);
			loopRegulator++;
			break;
		case 7:
			secondPointY = vertices->at(i);
			loopRegulator++;
			break;
		case 8:
			secondPointZ = vertices->at(i);

			// First triangle
			vertices->push_back(firstPointX);
			vertices->push_back(firstPointY + _height);
			vertices->push_back(firstPointZ);
			vertices->push_back(secondPointX);
			vertices->push_back(secondPointY + _height);
			vertices->push_back(secondPointZ);
			vertices->push_back(firstPointX);
			vertices->push_back(firstPointY);
			vertices->push_back(firstPointZ);

			// Second triangle
			vertices->push_back(secondPointX);
			vertices->push_back(secondPointY + _height);
			vertices->push_back(secondPointZ);
			vertices->push_back(secondPointX);
			vertices->push_back(secondPointY);
			vertices->push_back(secondPointZ);
			vertices->push_back(firstPointX);
			vertices->push_back(firstPointY);
			vertices->push_back(firstPointZ);

			loopRegulator = 0;
			break;
		default:
			loopRegulator++;
			break;
		}

	}

	int size = vertices->size();
	for (int i = 0; i < size; i++) { indices->push_back(i); }

}

