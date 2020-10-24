#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"
#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"

class GameObject;

struct Texture
{
	Texture();
	Texture(GLenum _textureType, const std::string& FileName);

	void SetAttributes();

	std::string fileName;
	GLenum textureType;
	GLuint textureId;
};


struct SubMeshes {

public:

	SubMeshes();
	~SubMeshes();

	GLuint vertexId;
	GLuint indexId;
	GLuint normalsId;
	GLuint textureCoordId;
	uint materialId;

	int vertexVectorSize;
	int indexVectorSize;
	int normalVectorSize;
	int textureCoordVectorSize;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> textureCoord;
	std::vector<uint> indices;
	Texture diffuseTexture;
};

class Mesh : public Component {

public:

	Mesh(int _id, GameObject* _gameObject);
	~Mesh();

	void Update();

public:

	std::vector<SubMeshes> subMeshes;

};

#endif