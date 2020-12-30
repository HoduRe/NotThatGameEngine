#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"
#include "OpenGLFuncionality.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject;


class Mesh : public Component {

public:

	Mesh(long long int _id, GameObject* _gameObject);
	~Mesh();

	void SetIsAnimation(bool boolean);
	void SetVertices(float* _vertices);
	void SetIndices(GLuint* _indices);
	void SetNormals(float* _normals);
	void SetTextureCoord(float* _textureCoord);
	void CalculateBoundingBoxes();

	GLuint DebugNormals();

public:

	GLuint vertexId;
	GLuint indexId;
	GLuint normalsId;
	GLuint textureCoordId;

	GLuint debugNormalsId;

	float* vertices;
	float* normals;
	float* textureCoord;
	GLuint* indices;
	int vertexSize;
	int indexSize;
	int normalsSize;
	int textureCoordSize;

	std::vector<float> debugNormals;
	bool paintNormals;
	bool showBoxes;
	bool showAllBones;
	bool isAnimatedWithBones;

	AABB boundingBox;

	int* boneIDs;
	float* boneWeights;
	bool* boneDisplayVec;

	int boneIDsSize;
	int boneWeightsSize;
	int boneDisplayVecSize;

	std::vector<std::string> boneNamesVec;
	std::vector<float4x4> boneOffsetMatrixVec;

	// This would be a pointer to mesh Resource... if I had of that ;)

	GLuint vertexIdANIMATION;
	GLuint normalsIdANIMATION;

	float* verticesANIMATION;
	float* normalsANIMATION;

};

#endif