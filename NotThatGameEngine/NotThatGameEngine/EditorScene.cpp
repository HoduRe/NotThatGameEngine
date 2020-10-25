#include "Application.h"

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


EditorScene::EditorScene(Application* app, bool start_enabled) : Module(app, start_enabled), gameObjectIdCount(0), rootGameObjectsVec(), stream() {}


EditorScene::~EditorScene() {

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) {
		delete rootGameObjectsVec[i];
		rootGameObjectsVec[i] = nullptr;
	}
}


bool EditorScene::Init() {

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;

}


bool EditorScene::Start()
{

	AddGameObjectByLoadingModel("Library/Meshes/BakerHouse.fbx", "Baker house");

	return true;
}


update_status EditorScene::PreUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


update_status EditorScene::Update(float dt)
{

	int size = rootGameObjectsVec.size();
	for (int i = 0; i < size; i++) { rootGameObjectsVec[i]->Update(); }

	return update_status::UPDATE_CONTINUE;

}


update_status EditorScene::PostUpdate(float dt) {

	int size = rootGameObjectsVec.size();
	for (int i = 0; i < size; i++) { rootGameObjectsVec[i]->PostUpdate(); }

	DeleteRootGameObjects();

	return update_status::UPDATE_CONTINUE;

}


bool EditorScene::CleanUp() {
	
	aiDetachAllLogStreams();
	return true;

}


int EditorScene::GenerateId() { return gameObjectIdCount++; }


bool EditorScene::AddGameObject(GameObject* newObject) {

	bool ret = false;

	if (newObject->parent == nullptr) {
		rootGameObjectsVec.push_back(newObject);
		return true;
	}
	else {
		int size = rootGameObjectsVec.size();
		for (int i = 0; i < size; i++) {
			if (rootGameObjectsVec[i] == newObject->parent) { rootGameObjectsVec[i]->childs.push_back(newObject); }
			else {
				ret = rootGameObjectsVec[i]->AddGameObjectByParent(newObject);
				if (ret) { return ret; }
			}
		}
	}

	LOG("GameObject didn't find root / node to be added into.");

	return ret;

}


GameObject* EditorScene::AddGameObjectByLoadingModel(const char* path, const char* objectName, GameObject* parent, bool enabled) {

	if (objectName == nullptr) { objectName = path; }
	GameObject* newObject = new GameObject(GenerateId(), objectName, parent, enabled);
	Mesh* mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);

	const aiScene* scene;
	Assimp::Importer importer;
	SubMeshes* subMesh;

	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr || scene->HasMeshes() == false) { LOG("Error loading scene % s", path); }

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		subMesh = new SubMeshes();
		mesh->subMeshes.push_back(*subMesh);
		subMesh = &mesh->subMeshes[i];

		const aiMesh* paiMesh = (aiMesh*)scene->mMeshes[i];
		//subMesh->materialId = paiMesh->mMaterialIndex;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D ZeroColor(80.0f, 80.0f, 80.0f, 1.0f);

		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {		// Vertices
			const aiVector3D* pPos = &(paiMesh->mVertices[j]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]); //: &Zero3D	// There are the same normals as there are vertices, so we don't need a loop for them
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;	// Same as above
			const aiColor4D* pColor = paiMesh->HasVertexColors(0) ? (paiMesh->mColors[j]) : &ZeroColor;	// Same as above

			subMesh->vertices.push_back(pPos->x);
			subMesh->vertices.push_back(pPos->y);
			subMesh->vertices.push_back(pPos->z);
			subMesh->textureCoord.push_back(pTexCoord->x);
			subMesh->textureCoord.push_back(pTexCoord->y);
			subMesh->normals.push_back(pNormal->x);
			subMesh->normals.push_back(pNormal->y);
			subMesh->normals.push_back(pNormal->z);
		}

		LOG("New mesh with %d vertices", subMesh->vertices.size());

		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
			const aiFace& Face = paiMesh->mFaces[j];
			assert(Face.mNumIndices == 3);
			subMesh->indices.push_back(Face.mIndices[0]);
			subMesh->indices.push_back(Face.mIndices[1]);
			subMesh->indices.push_back(Face.mIndices[2]);
		}

		// TODO: have an openGL module / functionality (maybe just do a sepparate OpenGLInitialization file to do ALL OpenGl shit there) that intitalizes a buffer with vertices and index. Seriously, having to Bind and UnBind Buffers all the time is a pain in the ass. Even if it's just a functionality and not a module

		LoadVertexBuffer(&subMesh->vertexId, sizeof(float) * subMesh->vertices.size(), subMesh->vertices.data());
		LoadNormalBuffer(&subMesh->normalsId, sizeof(float) * subMesh->normals.size(), subMesh->normals.data());
		LoadTextureCoordBuffer(&subMesh->textureCoordId, sizeof(float) * subMesh->textureCoord.size(), subMesh->textureCoord.data());
		LoadIndexBuffer(&subMesh->indexId, sizeof(uint) * subMesh->indices.size(), subMesh->indices.data());

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

	AddGameObject(newObject);

	return newObject;

}


void EditorScene::DeleteGameObject(int id) {

	bool ret = false;

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) {
		if (rootGameObjectsVec[i]->id == id) { rootGameObjectsVec[i]->SetDeleteGameObject(); }
		else {
			ret = rootGameObjectsVec[i]->CheckChildDeletionById(id);
			if (ret) { return; }
		}
	}

}


void EditorScene::DeleteRootGameObjects() {

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) { if (rootGameObjectsVec[i]->deleteGameObject) { rootGameObjectsVec.erase(rootGameObjectsVec.begin() + i); } }

}


bool EditorScene::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

	default:
		break;
	}

	return false;
}
