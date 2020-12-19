#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"
#include "OpenGLFuncionality.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject;

class Bone {

public:

	Bone(std::string _name, float4x4 _matrix);
	~Bone();

public:

	std::string name;
	float4x4 offsetMatrix;
	std::map<uint, float> vertexWeights;
	bool showBone;

	void AddWeight(std::pair<uint, float>(_weight));

};


class Mesh : public Component {

public:

	Mesh(long long int _id, GameObject* _gameObject);
	~Mesh();

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
	std::vector<Bone> boneVec;

	std::vector<float> debugNormals;
	bool paintNormals;
	bool showAllBones;

	AABB boundingBox;

};

#endif