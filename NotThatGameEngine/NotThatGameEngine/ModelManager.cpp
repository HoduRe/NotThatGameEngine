#include "Application.h"

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


ModelManager::ModelManager(Application* app, bool start_enabled) : Module(app, start_enabled), stream(), testMesh() {}


ModelManager::~ModelManager() {}


bool ModelManager::Init() {			// OpenGL has not been initialized yet

	bool ret = true;

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}


bool ModelManager::Start() {

	//LoadModel("Library/Models/warrior.FBX");
	LoadModel("Library/Models/BakerHouse.fbx");

	return true;
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

	const aiScene* scene;
	Assimp::Importer importer;

	scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr || scene->HasMeshes() == false) { LOG("Error loading scene % s", path); }

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		testMesh.subMeshes.push_back(SubMeshes());

		const aiMesh* paiMesh = (aiMesh*)scene->mMeshes[i];
		testMesh.subMeshes[i].materialId = paiMesh->mMaterialIndex;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D ZeroColor(80.0f, 80.0f, 80.0f, 1.0f);

		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {		// Vertices
			const aiVector3D* pPos = &(paiMesh->mVertices[j]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]) /*: &Zero3D*/;	// There are the same normals as there are vertices, so we don't need a loop for them
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;	// Same as above
			const aiColor4D* pColor = paiMesh->HasVertexColors(0) ? (paiMesh->mColors[j]) : &ZeroColor;	// Same as above

			testMesh.subMeshes[i].vertices.push_back(pPos->x);
			testMesh.subMeshes[i].vertices.push_back(pPos->y);
			testMesh.subMeshes[i].vertices.push_back(pPos->z);
			testMesh.subMeshes[i].textureCoord.push_back(pTexCoord->x);
			testMesh.subMeshes[i].textureCoord.push_back(pTexCoord->y);
			testMesh.subMeshes[i].normals.push_back(pNormal->x);
			testMesh.subMeshes[i].normals.push_back(pNormal->y);
			testMesh.subMeshes[i].normals.push_back(pNormal->z);
			testMesh.subMeshes[i].colors.push_back(pColor->r);
			testMesh.subMeshes[i].colors.push_back(pColor->g);
			testMesh.subMeshes[i].colors.push_back(pColor->b);
			testMesh.subMeshes[i].colors.push_back(pColor->a);

		}

		testMesh.subMeshes[i].vertexVectorSize = testMesh.subMeshes[i].vertices.size();
		testMesh.subMeshes[i].normalVectorSize = testMesh.subMeshes[i].normals.size();
		testMesh.subMeshes[i].textureCoordVectorSize = testMesh.subMeshes[i].textureCoord.size();

		LOG("New mesh with %d vertices", testMesh.subMeshes[i].vertexVectorSize);

		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
			const aiFace& Face = paiMesh->mFaces[j];
			assert(Face.mNumIndices == 3);
			testMesh.subMeshes[i].indices.push_back(Face.mIndices[0]);
			testMesh.subMeshes[i].indices.push_back(Face.mIndices[1]);
			testMesh.subMeshes[i].indices.push_back(Face.mIndices[2]);
		}

		testMesh.subMeshes[i].indexVectorSize = testMesh.subMeshes[i].indices.size();

		// TODO: have an openGL module / functionality (maybe just do a sepparate OpenGLInitialization file to do ALL OpenGl shit there) that intitalizes a buffer with vertices and index. Seriously, having to Bind and UnBind Buffers all the time is a pain in the ass. Even if it's just a functionality and not a module
		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].vertexId);
		glBindBuffer(GL_ARRAY_BUFFER, testMesh.subMeshes[i].vertexId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * testMesh.subMeshes[i].vertexVectorSize, testMesh.subMeshes[i].vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].normalsId);
		glBindBuffer(GL_NORMAL_ARRAY, testMesh.subMeshes[i].normalsId);
		glBufferData(GL_NORMAL_ARRAY, sizeof(float) * testMesh.subMeshes[i].normalVectorSize, testMesh.subMeshes[i].normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_NORMAL_ARRAY, 0);

		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].textureCoordId);
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, testMesh.subMeshes[i].textureCoordId);
		glBufferData(GL_TEXTURE_COORD_ARRAY, sizeof(float) * testMesh.subMeshes[i].textureCoordVectorSize, testMesh.subMeshes[i].textureCoord.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);

		glGenBuffers(1, (GLuint*)&testMesh.subMeshes[i].indexId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testMesh.subMeshes[i].indexId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * testMesh.subMeshes[i].indexVectorSize, testMesh.subMeshes[i].indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	// Init materials
/*	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

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
	}*/


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


SubMeshes::SubMeshes() : vertexId(0), indexId(0), normalsId(0), textureCoordId(0), materialId(0), vertexVectorSize(0), indexVectorSize(0),
normalVectorSize(0), textureCoordVectorSize(0), vertices(), normals(), textureCoord(), indices(), colors(0) {}


SubMeshes::~SubMeshes() {
	vertices.clear();
	normals.clear();
	textureCoord.clear();
	indices.clear();
}


