#include "Mesh.h"

Bone::Bone(std::string _name, float4x4 _matrix) : name(_name), offsetMatrix(_matrix), vertexWeights(), showBone(false) {}


Bone::~Bone() {}


void Bone::AddWeight(std::pair<uint, float>(_weight)) { vertexWeights.insert(_weight); }


Mesh::Mesh(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MESH), vertexId(0), indexId(0), normalsId(0), textureCoordId(0),
vertices(), normals(), textureCoord(), indices(), paintNormals(false), debugNormalsId(0), debugNormals(), boneVec(), showAllBones(false)
{
	boundingBox.SetNegativeInfinity();
}


Mesh::~Mesh() {

	vertices.clear();
	normals.clear();
	textureCoord.clear();
	indices.clear();

	glDeleteBuffers(1, &vertexId);
	glDeleteBuffers(1, &normalsId);
	glDeleteBuffers(1, &textureCoordId);
	glDeleteBuffers(1, &indexId);

}


void Mesh::SetVertices(std::vector<float> _vertices) {

	vertices = _vertices;
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &vertexId, vertices.size(), vertices.data());
	CalculateBoundingBoxes();

}


void Mesh::SetIndices(std::vector<GLuint> _indices) {

	indices = _indices;
	OpenGLFunctionality::LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &indexId, indices.size(), indices.data());

}


void Mesh::SetNormals(std::vector<float> _normals) {

	normals = _normals;
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &normalsId, normals.size(), normals.data());

}


void Mesh::SetTextureCoord(std::vector<float> _textureCoord)
{

	textureCoord = _textureCoord;
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &textureCoordId, textureCoord.size(), textureCoord.data());

}


GLuint Mesh::DebugNormals() {

	int verticesSize = vertices.size();

	if (normals.size() != 0 && verticesSize != 0) {

		if (debugNormalsId != 0) { return debugNormalsId; }

		else {

			for (int i = 0; i < verticesSize; i += 3) {

				debugNormals.push_back(vertices[i]);
				debugNormals.push_back(vertices[i + 1]);
				debugNormals.push_back(vertices[i + 2]);
				debugNormals.push_back(vertices[i] + normals[i]);
				debugNormals.push_back(vertices[i + 1] + normals[i + 1]);
				debugNormals.push_back(vertices[i + 2] + normals[i + 2]);

			}

			OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &debugNormalsId, debugNormals.size(), debugNormals.data());

		}

	}

	else { return 0; }

}


void Mesh::CalculateBoundingBoxes() { for (int i = 0; i < vertices.size(); i += 3) { boundingBox.Enclose(float3(vertices[i], vertices[i + 1], vertices[i + 2])); } }

