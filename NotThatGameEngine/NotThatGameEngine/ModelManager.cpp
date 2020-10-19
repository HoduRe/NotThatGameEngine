#include "Application.h"
#include "ModelManager.h"

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


ModelManager::ModelManager(Application* app, bool start_enabled) : Module(app, start_enabled), stream(), testMesh() {}


ModelManager::~ModelManager() {}


bool ModelManager::Init() {

	bool ret = true;

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	LoadModel("/Library/Meshes/warrior/warrior.FBX");

	return ret;
}


bool ModelManager::CleanUp() {

	aiDetachAllLogStreams();

	return true;
}


update_status ModelManager::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ModelManager::Update(float dt) {

	CheckListener(this);

	return update_status::UPDATE_CONTINUE;
}


update_status ModelManager::PostUpdate(float dt) {

	CheckListener(this);

	return update_status::UPDATE_CONTINUE;
}


void ModelManager::ExecuteEvent(EVENT_ENUM eventId) {


}


void ModelManager::LoadModel(std::string path) {

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes()) { aiReleaseImport(scene); }
	else { LOG("Error loading scene % s", path); }

	Assimp::Importer importer;

	scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	//
	testMesh.subMeshes.resize(scene->mNumMeshes);
	//m_Textures.resize(pScene->mNumMaterials);

	for (unsigned int i = 0; i < testMesh.subMeshes.size(); i++) {
		const aiMesh* paiMesh = scene->mMeshes[i];
		testMesh.subMeshes[i].MaterialIndex = paiMesh->mMaterialIndex;

		std::vector<float> vertices;
		std::vector<uint> indices;
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {		// Vertices
			const aiVector3D* pPos = &(paiMesh->mVertices[i]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[i]) /*: &Zero3D*/;
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

			vertices.push_back(pPos->x);
			vertices.push_back(pPos->y);
			vertices.push_back(pPos->z);
			vertices.push_back(pTexCoord->x);
			vertices.push_back(pTexCoord->y);
			vertices.push_back(pNormal->x);
			vertices.push_back(pNormal->y);
			vertices.push_back(pNormal->z);
		}

		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {		// Indices
			const aiFace& Face = paiMesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			indices.push_back(Face.mIndices[0]);
			indices.push_back(Face.mIndices[1]);
			indices.push_back(Face.mIndices[2]);
		}

		// TODO: have an openGL module / functionality (maybe just do a sepparate OpenGLInitialization file to do ALL OpenGl shit there) that intitalizes a buffer with vertices and index.
		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, testMesh.subMeshes[i].vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].indexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testMesh.subMeshes[i].indexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	// Init materials
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = scene->mMaterials[i];
		testMesh.textures[i] = NULL;
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string FullPath = path + "/" + Path.data;
				testMesh.textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

				if (!testMesh.textures[i]->Load()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete testMesh.textures[i];
					testMesh.textures[i] = NULL;
				}
			}
		}
		if (!testMesh.textures[i]) {
			testMesh.textures[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");
			testMesh.textures[i]->Load();
		}
	}

	/*
	testMesh.numVertices = mesh.mNumVertices;
	testMesh.vertices = new float[testMesh.numVertices * 3];
	memcpy(testMesh.vertices, aiMesh->mVertices, sizeof(float) * testMesh.numVertices * 3);
	*/
	
	LOG("New mesh with %d vertices", testMesh.numVertices);
}


Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
	textureTarget = TextureTarget;
	fileName = FileName;
}


bool Texture::Load() {

	glGenTextures(1, &textureId);
	glBindTexture(textureTarget, textureId);
//	glTexImage2D(textureTarget, 0, GL_RGBA, m_image.columns(), m_image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(textureTarget, 0);

	return true;
}


void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(textureTarget, textureId);
}


