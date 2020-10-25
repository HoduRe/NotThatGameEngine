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
	Mesh* mesh = nullptr;
	Material* material = nullptr;

	const aiScene* scene;
	Assimp::Importer importer;

	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr || scene->HasMeshes() == false) { LOG("Error loading scene % s", path); }

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH); 

		const aiMesh* paiMesh = (aiMesh*)scene->mMeshes[i];
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D ZeroColor(80.0f, 80.0f, 80.0f, 1.0f);

		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {		// Vertices
			const aiVector3D* pPos = &(paiMesh->mVertices[j]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]); //: &Zero3D	// There are the same normals as there are vertices, so we don't need a loop for them
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;	// Same as above
			const aiColor4D* pColor = paiMesh->HasVertexColors(0) ? (paiMesh->mColors[j]) : &ZeroColor;	// Same as above

			mesh->vertices.push_back(pPos->x);
			mesh->vertices.push_back(pPos->y);
			mesh->vertices.push_back(pPos->z);
			mesh->textureCoord.push_back(pTexCoord->x);
			mesh->textureCoord.push_back(pTexCoord->y);
			mesh->normals.push_back(pNormal->x);
			mesh->normals.push_back(pNormal->y);
			mesh->normals.push_back(pNormal->z);
		}

		LOG("New mesh with %d vertices", mesh->vertices.size());

		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
			const aiFace& Face = paiMesh->mFaces[j];
			assert(Face.mNumIndices == 3);
			mesh->indices.push_back(Face.mIndices[0]);
			mesh->indices.push_back(Face.mIndices[1]);
			mesh->indices.push_back(Face.mIndices[2]);
		}

		LoadVertexBuffer(&mesh->vertexId, sizeof(float) * mesh->vertices.size(), mesh->vertices.data());
		LoadNormalBuffer(&mesh->normalsId, sizeof(float) * mesh->normals.size(), mesh->normals.data());
		LoadTextureCoordBuffer(&mesh->textureCoordId, sizeof(float) * mesh->textureCoord.size(), mesh->textureCoord.data());
		LoadIndicesBuffer(&mesh->indexId, sizeof(uint) * mesh->indices.size(), mesh->indices.data());

	}

	// Init materials
	if (scene->mNumMaterials > 0) {

		material = (Material*)newObject->AddComponent(COMPONENT_TYPE::MATERIAL);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

			const aiMaterial* pMaterial = scene->mMaterials[i];

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string FullPath = path + (std::string)"/" + Path.data;	// TODO: does this allow for textures loading from anywhere, or what is this path addition about? Basically check that loading a model with an inherent texture works
					material->textureIdVec.push_back(App->texture->LoadTexture(FullPath.c_str(), Path.data));
				}
			}
			else {
				// TODO: If there ain't no texture, put default texture
			}
		}
	}

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
