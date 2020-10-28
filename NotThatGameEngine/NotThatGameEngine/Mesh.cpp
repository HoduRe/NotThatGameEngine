#include "Mesh.h"

Mesh::Mesh(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MESH), vertexId(0), indexId(0), normalsId(0), textureCoordId(0),
vertices(), normals(), textureCoord(), indices(), materialId(0) {}


Mesh::~Mesh() {

	vertices.clear();
	normals.clear();
	textureCoord.clear();
	indices.clear();

}


void Mesh::Update() {}


void Mesh::SetVertices(std::vector<float> _vertices) {

	vertices = _vertices;
	LoadDataBufferFloat(GL_ARRAY_BUFFER, &vertexId, vertices.size(), vertices.data());

}


void Mesh::SetIndices(std::vector<uint> _indices) {

	indices = _indices;
	LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &indexId, indices.size(), indices.data());

}


void Mesh::SetNormals(std::vector<float> _normals) {

	normals = _normals;
	LoadDataBufferFloat(GL_NORMAL_ARRAY, &normalsId, normals.size(), normals.data());

}


void Mesh::SetTextureCoord(std::vector<float> _textureCoord)
{

	textureCoord = _textureCoord;
	LoadDataBufferFloat(GL_TEXTURE_COORD_ARRAY, &textureCoordId, textureCoord.size(), textureCoord.data());

}



