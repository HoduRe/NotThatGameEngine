#include "Application.h"
#include "ModelManager.h"

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


ModelManager::ModelManager(Application* app, bool start_enabled) : Module(app, start_enabled), stream(), testMesh() {}


ModelManager::~ModelManager() {}


bool ModelManager::Init() {

	bool ret = true;

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	LoadModel("Library/Meshes/warrior/warrior.FBX");

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

	App->renderer3D->DrawMesh(testMesh);

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

	testMesh.subMeshes.resize(scene->mNumMeshes);
	testMesh.textures.resize(scene->mNumMaterials);

	for (unsigned int i = 0; i < testMesh.subMeshes.size(); i++) {

		testMesh.subMeshes.push_back(SubMeshes());

		const aiMesh* paiMesh = scene->mMeshes[i];
		testMesh.subMeshes[i].materialId = paiMesh->mMaterialIndex;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		// testMesh.subMeshes[i].vertices.resize(paiMesh->mNumVertices);	TODO Do we kill this? It doesn't seem to help to the code explosion

		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {		// Vertices
			const aiVector3D* pPos = &(paiMesh->mVertices[j]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]) /*: &Zero3D*/;
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;

			testMesh.subMeshes[i].vertices.push_back(pPos->x);
			testMesh.subMeshes[i].vertices.push_back(pPos->y);
			testMesh.subMeshes[i].vertices.push_back(pPos->z);
			testMesh.subMeshes[i].vertices.push_back(pTexCoord->x);
			testMesh.subMeshes[i].vertices.push_back(pTexCoord->y);
			testMesh.subMeshes[i].vertices.push_back(pNormal->x);
			testMesh.subMeshes[i].vertices.push_back(pNormal->y);
			testMesh.subMeshes[i].vertices.push_back(pNormal->z);

		}

		testMesh.subMeshes[i].vertexVectorSize = testMesh.subMeshes[i].vertices.size();

		LOG("New mesh with %d vertices", testMesh.subMeshes[i].vertexVectorSize);

		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
			const aiFace& Face = paiMesh->mFaces[j];
			assert(Face.mNumIndices == 3);
			testMesh.subMeshes[i].indices.push_back(Face.mIndices[0]);
			testMesh.subMeshes[i].indices.push_back(Face.mIndices[1]);
			testMesh.subMeshes[i].indices.push_back(Face.mIndices[2]);
		}

		testMesh.subMeshes[i].indexVectorSize = testMesh.subMeshes[i].indices.size();

		unsigned int uinti;
		// TODO: have an openGL module / functionality (maybe just do a sepparate OpenGLInitialization file to do ALL OpenGl shit there) that intitalizes a buffer with vertices and index.
		glGenBuffers(1, (GLuint*)&uinti);
		glBindBuffer(GL_ARRAY_BUFFER, testMesh.subMeshes[i].vertexId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * testMesh.subMeshes[i].vertexVectorSize, testMesh.subMeshes[i].vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].indexId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testMesh.subMeshes[i].indexId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * testMesh.subMeshes[i].indexVectorSize, testMesh.subMeshes[i].indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	/*// Init materials
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

	TODO Textures */
	
}


Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
	textureTarget = TextureTarget;
	fileName = FileName;
}


bool Texture::Load() {

	glGenTextures(1, &textureId);
	glBindTexture(textureTarget, textureId);
	//glTexImage2D(textureTarget, 0, GL_RGBA, m_image.columns(), m_image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());	TODO Textures
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


