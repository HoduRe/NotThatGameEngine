#include "Application.h"

ManagerModel::ManagerModel(Application* app, bool start_enabled) : Module(app, start_enabled), testMesh(0, nullptr) {}


ManagerModel::~ManagerModel() {}


bool ManagerModel::Init() {			// OpenGL has not been initialized yet

	bool ret = true;

	return ret;
}


bool ManagerModel::Start() {

	//LoadModel("Library/Models/warrior.FBX");
	LoadModel("Library/Models/BakerHouse.fbx");

	return true;
}


bool ManagerModel::CleanUp() {

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

/*	const aiScene* scene;
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
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]); //: &Zero3D	// There are the same normals as there are vertices, so we don't need a loop for them
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
	}*/
	
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


