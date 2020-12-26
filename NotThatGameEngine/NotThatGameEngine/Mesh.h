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
	void SetVertices(std::vector<float> _vertices);
	void SetIndices(std::vector<GLuint> _indices);
	void SetNormals(std::vector<float> _normals);
	void SetTextureCoord(std::vector<float> _textureCoord);
	void CalculateBoundingBoxes();

	GLuint DebugNormals();

public:

	GLuint vertexId;
	GLuint indexId;
	GLuint normalsId;
	GLuint textureCoordId;

	GLuint debugNormalsId;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> textureCoord;
	std::vector<GLuint> indices;

	std::vector<float> debugNormals;
	bool paintNormals;
	bool showAllBones;
	bool isAnimated;

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

	std::vector<float> verticesANIMATION;
	std::vector<float> normalsANIMATION;

};

#endif