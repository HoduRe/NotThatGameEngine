#include "Application.h"
#include "ManagerResource.h"
#include "ManagerEvent.h"
#include "ManagerExternal.h"
#include "EditorScene.h"
#include "Save.h"
#include "Load.h"
#include "ModelImporter.h"
#include "PathNode.h"
#include "GameObject.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled), assetsMap(), libraryMap(), memoryMap() {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	PathNode assetsFiles = App->externalManager->GetAllFiles(ASSETS_PATH);

	CheckAssetsImported(assetsFiles);

	return true;

}


bool ResourceManager::Start()
{

	App->eventManager->EventRegister(EVENT_ENUM::SAVE_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::LOAD_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_DROPPED, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_LOADED, this);

	return true;
}


update_status ResourceManager::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::PostUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


bool ResourceManager::CleanUp() {

	return true;

}


void ResourceManager::LoadLibraryFiles() {

	std::vector<std::string> sceneVec;
	App->externalManager->GetAllFilesWithExtension(SCENES_PATH, EXTENSION_SCENES, sceneVec);

	std::vector<std::string> textureVec;
	App->externalManager->GetAllFilesWithExtension(TEXTURES_PATH, EXTENSION_TEXTURES, textureVec);

	for (int i = 0; i < textureVec.size(); i++) { LoadResourceByType(textureVec[i], ResourceEnum::TEXTURE); }

	if (sceneVec.size() != 0) { LoadResourceByType(sceneVec[0], ResourceEnum::SCENE); }	// We only want to load one scene
	else {

		LOG("No scene to be loaded.\n");
		return;

	}

	sceneVec.clear();
	textureVec.clear();

}


void ResourceManager::ImportAssetsFile(PathNode* child) {

	std::string extension;
	ResourceEnum type;
	bool loaded = IsLoadedInLibrary(&child->path, &type);
	if (loaded) {}
	else { ImportAssetResourceByType(child->path, type); }
	for (int i = 0; i < child->children.size(); i++) { ImportAssetsFile(&child->children[i]); }

}


void ResourceManager::CheckAssetsImported(PathNode loadingNode) {

	for (int i = 0; i < loadingNode.children.size(); i++) {

		if (loadingNode.children[i].isFile) { SearchFileInFileMap(&loadingNode.children[i].path); }

		CheckAssetsImported(loadingNode.children[i]);

	}

}


void ResourceManager::SearchFileInFileMap(std::string* filePath) {

	if(libraryMap.count(*filePath) == 1){}
	else{/*TODO: import from library*/}

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	std::string filePath;
	char* buffer;
	uint size;
	uint id;
	ResourceEnum type = ResourceEnum::NONE;

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		DataSaving::SaveScene(App);

		break;

	case EVENT_ENUM::LOAD_SCENE:

		LoadLibraryFiles();

		break;

	case EVENT_ENUM::FILE_DROPPED:
	case EVENT_ENUM::FILE_LOADED:

		filePath = (char*)var;
		IsLoadedInLibrary(&filePath, &type);

		size = App->externalManager->Load(filePath.c_str(), &buffer);

		if (size != 0) {

			switch (type) {

			case ResourceEnum::EXTERNAL_MODEL:
			case ResourceEnum::OWN_MODEL:

				ModelImporter::LoadNewModel(App, filePath.c_str(), buffer, size);
				break;

			case ResourceEnum::TEXTURE:

				id = DataLoading::LoadTexture(App, filePath.c_str(), buffer, size);
				App->eventManager->GenerateEvent(EVENT_ENUM::PUT_TEXTURE_TO_FOCUSED_MODEL, EVENT_ENUM::NULL_EVENT, (void*)id);
				break;

			case ResourceEnum::SCENE:

				DataLoading::LoadScene(App, buffer);

				break;

			case ResourceEnum::UNKNOWN:

				LOG("File with path %s, with readable format, has been dropped into the engine.", filePath.c_str());
				break;

			default:
				break;
			}

			return true;
		}
	}

}

ResourceEnum ResourceManager::CheckResourceType(std::string name, std::string* extension, std::string* fileName) {

	App->externalManager->SplitFilePath(name.c_str(), nullptr, fileName, extension);
	std::string dot = ".";

	*extension = dot + *extension;

	static_assert(static_cast<int>(ResourceEnum::UNKNOWN) == 8, "Code Needs Update");

	if (*extension == EXTENSION_SCENES) { return ResourceEnum::SCENE; }
	else if (*extension == EXTENSION_MODELS) { return ResourceEnum::OWN_MODEL; }
	else if (*extension == EXTENSION_MESHES) { return ResourceEnum::MESH; }
	else if (*extension == EXTENSION_MATERIALS) { return ResourceEnum::MATERIAL; }
	else if (*extension == EXTENSION_CAMERA) { return ResourceEnum::CAMERA; }
	else if (*extension == ".FBX" || *extension == ".fbx" || *extension == ".OBJ" || *extension == ".obj") { return ResourceEnum::EXTERNAL_MODEL; }
	else if (*extension == ".tga" || *extension == ".png" || *extension == ".jpg" || *extension == ".dds" || *extension == ".TGA"
		|| *extension == ".PNG" || *extension == ".JPG" || *extension == ".DDS" || *extension == EXTENSION_TEXTURES) {
		return ResourceEnum::TEXTURE;
	}

	return ResourceEnum::UNKNOWN;

}


void ResourceManager::LoadResourceByType(std::string name, ResourceEnum type) {

	ResourceEnum resourceType;
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	char* buffer = nullptr;
	std::string fileName;

	type == ResourceEnum::NONE ? resourceType = CheckResourceType(name, &std::string()) : resourceType = type;

	switch (resourceType) {

	case ResourceEnum::SCENE:

		App->externalManager->Load((SCENES_PATH + name + EXTENSION_SCENES).c_str(), &buffer);
		DataLoading::LoadScene(App, buffer);

		break;

	case ResourceEnum::OWN_MODEL:



		break;

	case ResourceEnum::MESH:

		App->externalManager->SplitFilePath(name.c_str(), nullptr, &fileName);
		mesh = (Mesh*)App->editorScene->FindGameObjectByComponent((long long int)std::stoi(fileName));

		if (mesh != nullptr) {

			App->externalManager->Load((MESHES_PATH + name + EXTENSION_MESHES).c_str(), &buffer);
			DataLoading::LoadMesh(buffer, mesh);

		}

		break;

	case ResourceEnum::MATERIAL:

		App->externalManager->SplitFilePath(name.c_str(), nullptr, &fileName);
		material = (Material*)App->editorScene->FindGameObjectByComponent((long long int)std::stoi(fileName));

		if (material != nullptr) {

			App->externalManager->Load((MATERIALS_PATH + name + EXTENSION_MATERIALS).c_str(), &buffer);
			DataLoading::LoadMaterial(App, buffer, material);

		}

		break;

	case ResourceEnum::TEXTURE:

		DataLoading::LoadTexture(App, (TEXTURES_PATH + name).c_str());

		break;

	case ResourceEnum::UNKNOWN:

		LOG("Unknown resource type for file %s.\n", name.c_str());

		break;

	default:
		break;
	}

	RELEASE_ARRAY(buffer);

}


void ResourceManager::ImportAssetResourceByType(std::string name, ResourceEnum type) {

	ResourceEnum resourceType;
	char* buffer = nullptr;
	std::string fileName;
	GameObject* gameObject;

	type == ResourceEnum::NONE ? resourceType = CheckResourceType(name, &std::string()) : resourceType = type;

	switch (resourceType) {

	case ResourceEnum::EXTERNAL_MODEL:

		App->externalManager->Load((ASSETS_PATH + name).c_str(), &buffer);
		gameObject = ModelImporter::LoadNewModel(App, (ASSETS_PATH + name).c_str());
		DataSaving::SaveModel(App, gameObject, gameObject->name);
		gameObject->SetDeleteGameObject();

		break;

	case ResourceEnum::TEXTURE:

		DataLoading::LoadTexture(App, name.c_str());

		break;

	case ResourceEnum::UNKNOWN:

		LOG("Unknown resource type for file %s.\n", name.c_str());

		break;

	default:
		break;
	}

	RELEASE_ARRAY(buffer);

}


std::string ResourceManager::GetPathFromType(ResourceEnum type) {

	switch (type) {

	case ResourceEnum::SCENE: return SCENES_PATH;

	case ResourceEnum::EXTERNAL_MODEL:
	case ResourceEnum::OWN_MODEL:
		return MODELS_PATH;

	case ResourceEnum::MESH: return MESHES_PATH;

	case ResourceEnum::MATERIAL: return MATERIALS_PATH;

	case ResourceEnum::TEXTURE: return TEXTURES_PATH;

	case ResourceEnum::UNKNOWN:
	default:
		return "";
	}

}


std::string ResourceManager::ConvertLoadExtension(ResourceEnum type, std::string extension) {

	switch (type) {

	case ResourceEnum::TEXTURE: return EXTENSION_TEXTURES;
	case ResourceEnum::EXTERNAL_MODEL: return EXTENSION_MODELS;

	default:
		break;

	}

	return extension;

}


bool ResourceManager::IsLoadedInLibrary(std::string* filePath, ResourceEnum* type) {

	std::string path;
	std::string fileName;
	std::string extension;

	*filePath = App->externalManager->NormalizePath(filePath->c_str());
	*filePath = App->externalManager->LocalizePath(*filePath);

	*type = CheckResourceType(*filePath, &extension, &fileName);
	path = GetPathFromType(*type);
	extension = ConvertLoadExtension(*type, extension);

	if (App->externalManager->Exists((path + fileName + extension).c_str())) {

		if (*type == ResourceEnum::EXTERNAL_MODEL) { extension = EXTENSION_MODELS; }
		*filePath = path + fileName + extension;
		return true;

	}

	return false;

}


std::string ResourceManager::FindPathFromFileName(std::string fileName, PathNode* node) {

	std::string returnName;

	if (node == nullptr) { node; }

	for (int i = 0; i < node->children.size(); i++) {

		if (node->children[i].localPath == fileName) { return App->externalManager->NormalizePath(node->children[i].path.c_str()); }
		else {

			returnName = FindPathFromFileName(fileName, &node->children[i]);
			if (returnName.empty() == false) { return returnName; }

		}

	}

	return "";

}


