#include "Application.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment ( lib, "Devil/libx86/DevIL.lib" )
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


ManagerModel::ManagerModel(Application* app, bool start_enabled) : Module(app, start_enabled), stream(), testMesh() {}


ManagerModel::~ManagerModel() {}


bool ManagerModel::Init() {			// OpenGL has not been initialized yet

	bool ret = true;

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilInit();
	iluInit();
	ilutInit();

	return ret;
}


bool ManagerModel::Start() {

	//LoadModel("Library/Models/warrior.FBX");
	LoadModel("Library/Models/BakerHouse.fbx");

	LoadTexture("Library/Textures/Baker_house.png");

	return true;
}


bool ManagerModel::CleanUp() {

	aiDetachAllLogStreams();

	return true;
}


update_status ManagerModel::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ManagerModel::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ManagerModel::PostUpdate(float dt) {

	App->renderer3D->DrawMesh(testMesh);

	return update_status::UPDATE_CONTINUE;
}


bool ManagerModel::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	return false;
}


void ManagerModel::LoadModel(const char* path) {

	const aiScene* scene;
	Assimp::Importer importer;

	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

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


uint ManagerModel::LoadTexture(const char* path, Texture* textureContainer, GLenum type) {

	uint imageTest;
	ilGenImages(1, &imageTest);
	ilBindImage(imageTest);
	if (ilLoadImage(path) == IL_TRUE) {}
	else { LOG("Image with id: %u failed to load.\n", imageTest); }

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &imageTest);
	glBindTexture(GL_TEXTURE_2D, imageTest);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &imageTest);

	return imageTest;
}


Texture::Texture() : textureType(), fileName(), textureId() {}


Texture::Texture(GLenum _textureType, const std::string& FileName): textureId(NULL), textureType( _textureType), fileName(FileName)
{
}


void Texture::SetAttributes() {
	// TODO I have no idea how I want to load and store textures. Do gameobject hierarchy and then we talk
}


SubMeshes::SubMeshes() : vertexId(0), indexId(0), normalsId(0), textureCoordId(0), materialId(0), vertexVectorSize(0), indexVectorSize(0),
normalVectorSize(0), textureCoordVectorSize(0), vertices(), normals(), textureCoord(), indices(), diffuseTexture() {}


SubMeshes::~SubMeshes() {
	vertices.clear();
	normals.clear();
	textureCoord.clear();
	indices.clear();
}


Mesh::Mesh(): subMeshes(0) {}


Mesh::~Mesh() { subMeshes.clear(); }
