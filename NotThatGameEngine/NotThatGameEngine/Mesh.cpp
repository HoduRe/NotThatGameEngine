#include "Mesh.h"

Mesh::Mesh(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MESH), subMeshes() {}


Mesh::~Mesh() { subMeshes.clear(); }


void Mesh::Update() {}


SubMeshes::SubMeshes() : vertexId(0), indexId(0), normalsId(0), textureCoordId(0), materialId(0), vertexVectorSize(0), indexVectorSize(0),
normalVectorSize(0), textureCoordVectorSize(0), vertices(), normals(), textureCoord(), indices(), diffuseTexture() {}


SubMeshes::~SubMeshes() {
	vertices.clear();
	normals.clear();
	textureCoord.clear();
	indices.clear();
}


Texture::Texture() : textureType(), fileName(), textureId() {}


Texture::Texture(GLenum _textureType, const std::string& FileName) : textureId(NULL), textureType(_textureType), fileName(FileName)
{
}


void Texture::SetAttributes() {
	// TODO I have no idea how I want to load and store textures. Do gameobject hierarchy and then we talk
}


