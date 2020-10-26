#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"
#include "OpenGLFuncionality.h"

class GameObject;

class Mesh : public Component {

public:

	Mesh(int _id, GameObject* _gameObject);
	~Mesh();

	void Update();
	void SetVertices(std::vector<float> _vertices);
	void SetIndices(std::vector<uint> _indices);
	void SetNormals(std::vector<float> _normals);
	void SetTextureCoord(std::vector<float> _textureCoord);

public:

	GLuint vertexId;
	GLuint indexId;
	GLuint normalsId;
	GLuint textureCoordId;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> textureCoord;
	std::vector<uint> indices;

};

#endif