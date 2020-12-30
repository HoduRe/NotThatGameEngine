#include "Mesh.h"

Mesh::Mesh(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MESH), vertexId(0), indexId(0), normalsId(0), textureCoordId(0),
vertices(nullptr), normals(nullptr), textureCoord(nullptr), indices(nullptr), paintNormals(false), debugNormalsId(0), debugNormals(), boneIDs(nullptr), boneWeights(nullptr),
boneNamesVec(), boneOffsetMatrixVec(), boneDisplayVec(nullptr), showAllBones(false), boneIDsSize(0), boneWeightsSize(0), boneDisplayVecSize(0),
vertexIdANIMATION(0), normalsIdANIMATION(0), verticesANIMATION(nullptr), normalsANIMATION(nullptr), isAnimatedWithBones(false), showBoxes(false),
vertexSize(0), indexSize(0), normalsSize(0), textureCoordSize(0)
{
	boundingBox.SetNegativeInfinity();
}


Mesh::~Mesh() {

	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(textureCoord);

	RELEASE_ARRAY(verticesANIMATION);
	RELEASE_ARRAY(normalsANIMATION);

	debugNormals.clear();
	RELEASE_ARRAY(boneIDs);
	RELEASE_ARRAY(boneWeights);
	RELEASE_ARRAY(boneDisplayVec);
	boneNamesVec.clear();
	boneOffsetMatrixVec.clear();

	glDeleteBuffers(1, &vertexId);
	glDeleteBuffers(1, &normalsId);
	glDeleteBuffers(1, &textureCoordId);
	glDeleteBuffers(1, &indexId);

	glDeleteBuffers(1, &vertexIdANIMATION);
	glDeleteBuffers(1, &normalsIdANIMATION);

}


void Mesh::SetIsAnimation(bool boolean) { isAnimatedWithBones = boolean; }


void Mesh::SetVertices(float* _vertices) {

	if(_vertices != nullptr){ vertices = _vertices; }
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &vertexId, vertexSize, vertices);
	CalculateBoundingBoxes();

}


void Mesh::SetIndices(GLuint* _indices) {

	if (_indices != nullptr) { indices = _indices; }
	OpenGLFunctionality::LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &indexId, indexSize, indices);

}


void Mesh::SetNormals(float* _normals) {

	if (_normals != nullptr) { normals = _normals; }
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &normalsId, normalsSize, normals);

}


void Mesh::SetTextureCoord(float* _textureCoord)
{

	if (_textureCoord != nullptr) { textureCoord = _textureCoord; }
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &textureCoordId, textureCoordSize, textureCoord);

}


GLuint Mesh::DebugNormals() {

	int verticesSize = vertexSize;

	if (normalsSize != 0 && verticesSize != 0) {

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

			return debugNormalsId;

		}

	}

	else { return 0; }

}


void Mesh::CalculateBoundingBoxes() { for (int i = 0; i < vertexSize; i += 3) { boundingBox.Enclose(float3(vertices[i], vertices[i + 1], vertices[i + 2])); } }



