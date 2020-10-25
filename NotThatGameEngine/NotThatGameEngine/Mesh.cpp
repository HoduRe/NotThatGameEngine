#include "Mesh.h"

Mesh::Mesh(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MESH), subMeshes() {}


Mesh::~Mesh() { subMeshes.clear(); }


void Mesh::Update() {}


SubMeshes::SubMeshes() : vertexId(0), indexId(0), normalsId(0), textureCoordId(0), vertices(), normals(), textureCoord(), indices() {}


SubMeshes::~SubMeshes() {
	vertices.clear();
	normals.clear();
	textureCoord.clear();
	indices.clear();
}


