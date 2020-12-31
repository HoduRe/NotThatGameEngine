#include "Application.h"
#include "ManagerResource.h"
#include "ManagerEvent.h"
#include "ManagerExternal.h"
#include "EditorScene.h"
#include "JsonManager.h"
#include "parson/parson.h"
#include "Save.h"
#include "Load.h"
#include "Import.h"
#include "PathNode.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Component.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled), assetsMap(), libraryMap() {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	App->eventManager->EventRegister(EVENT_ENUM::SAVE_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_DROPPED, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_LOADING, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_DRAGGED_IN_LOAD_WINDOW, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_DELETED, this);
	App->eventManager->EventRegister(EVENT_ENUM::GAMEOBJECT_LOADED, this);

	PathNode assetsFiles = App->externalManager->GetAllFiles(ASSETS_PATH);
	PathNode libraryFiles = App->externalManager->GetAllFiles(LIBRARY_PATH);

	DataLoading::LoadAssetsMap(App, &assetsMap);
	DataLoading::LoadLibraryMap(App, &libraryMap);
	CheckLibraryFiles(&libraryFiles);
	CheckAssetsImported(&assetsFiles);
	CheckDeletedAssets();

	std::string name = "Alex";
	if (assetsMap.count(name) == 1) { App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)assetsMap.find(name)->second.filePath.c_str()); }
	name = "Checker";
	if (assetsMap.count(name) == 1) { App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)assetsMap.find(name)->second.filePath.c_str()); }
	name = "Degenerate";
	if (assetsMap.count(name) == 1) { App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)assetsMap.find(name)->second.filePath.c_str()); }
	name = "Scene";
	if (assetsMap.count(name) == 1) { App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)assetsMap.find(name)->second.filePath.c_str()); }

	return true;

}


bool ResourceManager::Start() { return true; }


update_status ResourceManager::PreUpdate(float dt) { return update_status::UPDATE_CONTINUE; }


update_status ResourceManager::Update(float dt) { return update_status::UPDATE_CONTINUE; }


update_status ResourceManager::PostUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


bool ResourceManager::CleanUp() {

	DataSaving::SaveAssetsMap(App, &assetsMap);
	DataSaving::SaveLibraryMap(App, &libraryMap);

	return true;

}


void ResourceManager::CheckLibraryFiles(PathNode* loadingNode) {

	for (uint i = 0; i < loadingNode->children.size(); i++) {

		if (loadingNode->children[i].isFile) {

			std::string fileName, extension;
			App->externalManager->SplitFilePath(loadingNode->children[i].path.c_str(), nullptr, &fileName, &extension);

			if ("." + extension != EXTENSION_MAP && libraryMap.count(fileName) == 0) {

				libraryMap.insert(std::pair<std::string, LibraryInfo>(fileName, LibraryInfo(loadingNode->children[i].path, ResourceEnum(GetTypeByExtension(extension)))));

			}

		}

		CheckLibraryFiles(&loadingNode->children[i]);

	}

}


void ResourceManager::CheckAssetsImported(PathNode* loadingNode) {

	for (uint i = 0; i < loadingNode->children.size(); i++) {

		if (loadingNode->children[i].isFile) { SearchFileInFileMap(loadingNode->children[i].path); }

		CheckAssetsImported(&loadingNode->children[i]);

	}

}


void ResourceManager::SearchFileInFileMap(std::string filePath) {

	std::string fileName, extension;
	App->externalManager->SplitFilePath(filePath.c_str(), nullptr, &fileName, &extension);
	ResourceEnum type = GetTypeByExtension(extension);

	switch (type) {

	case ResourceEnum::EXTERNAL_MODEL:
	case ResourceEnum::TEXTURE:
	case ResourceEnum::SCENE:

		if (assetsMap.count(fileName) == 1) {

			assetsMap[fileName].checked = true;
			if (assetsMap[fileName].lastTimeChanged != App->externalManager->GetLastModTime(assetsMap.find(fileName)->second.filePath.c_str())) {

				ImportToLibrary(filePath, fileName, extension);
				assetsMap[fileName].lastTimeChanged = App->externalManager->GetLastModTime(assetsMap.find(fileName)->second.filePath.c_str());

			}

		}

		else { assetsMap.insert(std::pair<std::string, FileInfo>(fileName, FileInfo(filePath, App->idGenerator.Int(), App->externalManager->GetLastModTime(filePath.c_str()), true))); }

		if (libraryMap.count(fileName) == 0) { ImportToLibrary(filePath, fileName, extension); }

		break;

	default:
		break;

	}

}


void ResourceManager::ImportToLibrary(std::string filePath, std::string fileName, std::string extension) {

	ResourceEnum type = GetTypeByExtension(extension);
	std::string finalPath = ImportAssetResourceByType(filePath, fileName, type);
	libraryMap.insert(std::pair<std::string, LibraryInfo>(fileName, LibraryInfo(finalPath, type)));

}


std::string ResourceManager::ImportAssetResourceByType(std::string path, std::string name, ResourceEnum type) {

	char* buffer = nullptr;
	uint size = App->externalManager->Load(path.c_str(), &buffer);;
	std::string finalPath;

	if (size > 0) {

		switch (type) {

		case ResourceEnum::EXTERNAL_MODEL:

			finalPath = Importer::ImportNewModel(App, path.c_str(), buffer, size);

			break;

		case ResourceEnum::TEXTURE:

			size = App->externalManager->Load(path.c_str(), &buffer);
			if (size > 0) { finalPath = Importer::ImportTexture(App, name.c_str(), buffer, size); }

			break;

		case ResourceEnum::SCENE:

			if (libraryMap.count(name) == 1) { finalPath = ManageSceneFiles(path, buffer, libraryMap.find(name)->second.filePath); }
			else { finalPath = ManageSceneFiles(path, buffer, std::string()); }
			break;

		case ResourceEnum::UNKNOWN:

			LOG("Unknown resource type for file %s.\n", name.c_str());

			break;

		default:
			break;
		}

		RELEASE_ARRAY(buffer);

	}

	return finalPath;

}


void ResourceManager::CheckDeletedAssets() {

	for (std::map<std::string, FileInfo>::iterator it = assetsMap.begin(); it != assetsMap.end(); it) {

		if (!assetsMap.at(it->first).checked) {

			std::map<std::string, FileInfo>::iterator itAux = it;
			itAux++;

			DeleteLibraryFile(it->first);

			it = itAux;

		}

		else { it++; }

	}

}


void ResourceManager::DeleteLibraryFile(std::string fileName) {

	if (libraryMap.count(fileName) == 1) {

		if (libraryMap.find(fileName)->second.type == ResourceEnum::SCENE) { ManageSceneFiles(std::string(), nullptr, libraryMap.find(fileName)->second.filePath); }
		App->externalManager->RemoveFileByName(libraryMap.find(fileName)->second.filePath.c_str());

		libraryMap.erase(libraryMap.find(fileName));
		assetsMap.erase(assetsMap.find(fileName));

	}

}


void ResourceManager::LoadResourceByPath(std::string filePath, std::string fileName, std::string extension) {

	char* buffer;
	uint size;
	Component* component = nullptr;
	ResourceEnum type = ResourceEnum::NONE;

	filePath = IsLoadedInLibrary(&filePath);
	App->externalManager->SplitFilePath(filePath.c_str(), nullptr, &fileName, &extension);
	type = GetTypeByExtension(extension);

	GetComponentIfLoaded(&type, &component, fileName);

	size = App->externalManager->Load(filePath.c_str(), &buffer);

	if (size != 0) {

		switch (type) {

		case ResourceEnum::EXTERNAL_MODEL:

			Importer::ImportNewModel(App, filePath.c_str(), buffer, size);
			break;

		case ResourceEnum::OWN_MODEL:

			DataLoading::LoadModel(App, buffer);
			break;

		case ResourceEnum::TEXTURE:

			DataLoading::LoadTexture(App, filePath.c_str(), buffer, size);
			if (assetsMap.count(fileName) == 1) { App->eventManager->GenerateEvent(EVENT_ENUM::PUT_TEXTURE_TO_FOCUSED_MODEL, EVENT_ENUM::NULL_EVENT, (void*)assetsMap.find(fileName)->first.c_str()); }
			break;

		case ResourceEnum::SCENE:

			DataLoading::LoadScene(App, buffer);
			break;

		case ResourceEnum::MESH:

			DataLoading::LoadMesh(buffer, (Mesh*)component);
			break;

		case ResourceEnum::MATERIAL:

			DataLoading::LoadMaterial(App, buffer, (Material*)component);
			break;

		case ResourceEnum::CAMERA:

			DataLoading::LoadCamera(buffer, (Camera*)component);
			break;

		case ResourceEnum::ANIMATION:

			DataLoading::LoadAnimation((char*)buffer, App->editorScene->GetFocus()->animation);
			break;

		case ResourceEnum::UNKNOWN:
			break;

		default:
			break;
		}

		LOG("File with path %s loaded.", filePath.c_str());

	}

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {	// TODO: if var is corrupted, this will explode. It shouldn't. But

	std::string filePath, pathAux, fileName, extension;
	char* buffer = nullptr;

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		filePath = DataSaving::SaveScene(App);
		App->externalManager->Load(filePath.c_str(), &buffer);
		App->externalManager->SplitFilePath(filePath.c_str(), nullptr, &fileName);

		if (assetsMap.count(fileName) == 1) {
			assetsMap.insert(std::pair<std::string, FileInfo>(fileName, FileInfo(filePath, assetsMap.find(fileName)->second.id, App->externalManager->GetLastModTime(filePath.c_str()), true)));
		}
		else { assetsMap.insert(std::pair<std::string, FileInfo>(fileName, FileInfo(filePath, App->idGenerator.Int(), App->externalManager->GetLastModTime(filePath.c_str()), true))); }

		if (libraryMap.count(fileName) == 1) { ManageSceneFiles(filePath, buffer, libraryMap.find(fileName)->second.filePath); }
		else {

			pathAux = ManageSceneFiles(filePath, buffer, std::string());
			libraryMap.insert(std::pair<std::string, LibraryInfo>(fileName, LibraryInfo(pathAux, ResourceEnum::SCENE)));

		}

		break;

	case EVENT_ENUM::FILE_DROPPED:

		filePath = (char*)var;
		App->externalManager->SplitFilePath((const char*)var, nullptr, &fileName, &extension);

		if (assetsMap.count(fileName) == 0) {

			filePath = ASSETS_PATH + fileName + "." + extension;
			App->externalManager->DuplicateFile((const char*)var, (filePath).c_str());

		}

		else { filePath = assetsMap.find(fileName)->second.filePath; }	// The path we recieve when dropping is from C:\, not from Assets/ 

	case EVENT_ENUM::FILE_LOADING:

		filePath = (char*)var;
		App->externalManager->SplitFilePath((const char*)var, nullptr, &fileName, &extension);

		SearchFileInFileMap(filePath);
		LoadResourceByPath(filePath, fileName, extension);
		break;


	case EVENT_ENUM::FILE_DELETED:

		App->externalManager->SplitFilePath((const char*)var, nullptr, &fileName);
		DeleteLibraryFile(fileName);
		break;

	case EVENT_ENUM::FILE_DRAGGED_IN_LOAD_WINDOW:

		App->externalManager->SplitFilePath((const char*)var, nullptr, &fileName);
		if (assetsMap.count(fileName) == 1) {

			if (assetsMap.find(fileName)->second.filePath != (const char*)var) {

				App->externalManager->DuplicateFile(assetsMap.find(fileName)->second.filePath.c_str(), (const char*)var);
				App->externalManager->RemoveFileByName(assetsMap.find(fileName)->second.filePath.c_str());
				assetsMap.find(fileName)->second.filePath = (const char*)var;

			}

		}

		break;

	case EVENT_ENUM::GAMEOBJECT_LOADED:

		ManageGameObjectLoading((GameObject*)var);
		break;

	default:
		break;

	}

	RELEASE_ARRAY(buffer);

	return true;

}


std::string ResourceManager::IsLoadedInLibrary(std::string* filePath) {

	std::string fileName;
	App->externalManager->SplitFilePath(filePath->c_str(), nullptr, &fileName);
	if (libraryMap.count(fileName) == 1) { return libraryMap.find(fileName)->second.filePath; }
	return *filePath;

}


void ResourceManager::ManageGameObjectLoading(GameObject* gameObject) {

	// When a gameObject / Model is loaded, do things here with the Memory maps. gameObject is the root GameObject

	long long int id = App->idGenerator.Int();
	gameObject->id = id;

	if (gameObject->transform != nullptr) { gameObject->transform->id = App->idGenerator.Int(); }
	if (gameObject->mesh != nullptr) { gameObject->mesh->id = App->idGenerator.Int(); }
	if (gameObject->material != nullptr) { gameObject->material->id = App->idGenerator.Int(); }
	if (gameObject->camera != nullptr) { gameObject->camera->id = App->idGenerator.Int(); }

	for (uint i = 0; i < gameObject->childs.size(); i++) { ManageGameObjectLoading(gameObject->childs[i]); }


}


std::string ResourceManager::ManageSceneFiles(std::string assetsScenePath, char* bufferAssets, std::string libraryScenePath) {

	std::string fileName, libraryAuxPath, componentPath;
	std::vector<int> assetsIDs, libraryIDs;

	if (assetsScenePath.empty()) { App->externalManager->SplitFilePath(libraryScenePath.c_str(), nullptr, &fileName); }	// When scene has been deleted from Assets/
	else { App->externalManager->SplitFilePath(assetsScenePath.c_str(), nullptr, &fileName); }

	libraryAuxPath = LIBRARY_SCENES_PATH + fileName + EXTENSION_SCENES;

	if (!libraryScenePath.empty()) {	// Enters when the saved scene has been re-saved: we check if there has been changes within, correct them, and delete deprecated components

		char* bufferLibrary;

		int size = App->externalManager->Load(libraryAuxPath.c_str(), &bufferLibrary);
		if (bufferAssets != nullptr) { assetsIDs = GetSceneComponents(bufferAssets); }	// If there is no Scene in Assets/, buffer is nullptr
		if (size != 0) { libraryIDs = GetSceneComponents(bufferLibrary); }

		for (uint i = 0; i < assetsIDs.size(); i += 2) {	// Not as redundant to re-loop the previously looped info as it seems

			for (int j = 0; j < libraryIDs.size(); j += 2) {

				if (assetsIDs[i] == libraryIDs[j]) {

					assetsIDs.erase(assetsIDs.begin() + (i + 1));
					assetsIDs.erase(assetsIDs.begin() + i);
					i -= 2;
					libraryIDs.erase(libraryIDs.begin() + (j + 1));
					libraryIDs.erase(libraryIDs.begin() + j);
					j = libraryIDs.size();

				}

			}

		}

	}

	for (uint i = 0; i < assetsIDs.size(); i += 2) {

		ResourceEnum type = ConvertComponentTypeToResourceType((COMPONENT_TYPE*)&assetsIDs[i + 1]);
		libraryMap.insert(std::pair<std::string, LibraryInfo>(fileName, LibraryInfo(libraryAuxPath, type)));

	}

	for (uint i = 0; i < libraryIDs.size(); i += 2) {

		ResourceEnum type = ConvertComponentTypeToResourceType((COMPONENT_TYPE*)&libraryIDs[i + 1]);
		componentPath = GetPathByType(type) + std::to_string(libraryIDs[i]) + GetExtensionByType(type);
		App->externalManager->RemoveFileByName(componentPath.c_str());
		if (libraryMap.count(std::to_string(libraryIDs[i])) == 1) { libraryMap.erase(libraryMap.find(std::to_string(libraryIDs[i]))); }

	}

	if (assetsScenePath.empty() == false) { App->externalManager->DuplicateFile(assetsScenePath.c_str(), libraryAuxPath.c_str()); }

	return libraryAuxPath;

}


std::vector<int> ResourceManager::GetSceneComponents(char* buffer) {

	std::vector<int> componentIDs;

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* objectsNode(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));
	int size = JsonManager::GetArraySize(objectsNode);

	for (uint i = 0; i < size; i++) {

		JSON_Object* nodeObject = json_array_get_object(objectsNode, i);
		JSON_Array* componentNode(json_object_get_array(nodeObject, JSON_NODE_COMPONENTS));
		int sizeComponents = JsonManager::GetArraySize(componentNode);

		for (int j = 0; j < sizeComponents; j++) {

			JSON_Object* nodeComponent = json_array_get_object(componentNode, j);
			componentIDs.push_back(json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_ID));
			componentIDs.push_back(json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_TYPE));

		}

	}

	return componentIDs;

}


ResourceEnum ResourceManager::GetTypeByExtension(std::string extension) {

	extension = "." + extension;

	static_assert(static_cast<int>(ResourceEnum::UNKNOWN) == 9, "Code Needs Update");

	if (extension == EXTENSION_SCENES) { return ResourceEnum::SCENE; }
	else if (extension == EXTENSION_MODELS) { return ResourceEnum::OWN_MODEL; }
	else if (extension == EXTENSION_MESHES) { return ResourceEnum::MESH; }
	else if (extension == EXTENSION_MATERIALS) { return ResourceEnum::MATERIAL; }
	else if (extension == EXTENSION_CAMERA) { return ResourceEnum::CAMERA; }
	else if (extension == EXTENSION_ANIMATIONS) { return ResourceEnum::ANIMATION; }
	else if (extension == ".FBX" || extension == ".fbx" || extension == ".OBJ" || extension == ".obj") { return ResourceEnum::EXTERNAL_MODEL; }
	else if (extension == ".tga" || extension == ".png" || extension == ".jpg" || extension == ".dds" || extension == ".TGA"
		|| extension == ".PNG" || extension == ".JPG" || extension == ".DDS" || extension == ".jpeg" || extension == ".JPEG" || extension == EXTENSION_TEXTURES) {
		return ResourceEnum::TEXTURE;
	}

	return ResourceEnum::UNKNOWN;

}


std::string ResourceManager::GetExtensionByType(ResourceEnum type) {

	if (type == ResourceEnum::SCENE) { return EXTENSION_SCENES; }
	else if (type == ResourceEnum::OWN_MODEL) { return EXTENSION_MODELS; }
	else if (type == ResourceEnum::MESH) { return EXTENSION_MESHES; }
	else if (type == ResourceEnum::MATERIAL) { return EXTENSION_MATERIALS; }
	else if (type == ResourceEnum::CAMERA) { return EXTENSION_CAMERA; }
	else if (type == ResourceEnum::ANIMATION) { return EXTENSION_ANIMATIONS; }
	else if (type == ResourceEnum::EXTERNAL_MODEL) { return ".fbx"; }
	else if (type == ResourceEnum::TEXTURE) { return EXTENSION_TEXTURES; }

	return std::string();

}


std::string ResourceManager::GetPathByType(ResourceEnum type) {

	if (type == ResourceEnum::SCENE) { return SCENES_PATH; }
	else if (type == ResourceEnum::OWN_MODEL) { return MODELS_PATH; }
	else if (type == ResourceEnum::MESH) { return MESHES_PATH; }
	else if (type == ResourceEnum::MATERIAL) { return MATERIALS_PATH; }
	else if (type == ResourceEnum::CAMERA) { return CAMERAS_PATH; }
	else if (type == ResourceEnum::ANIMATION) { return ANIMATIONS_PATH; }
	else if (type == ResourceEnum::TEXTURE) { return TEXTURES_PATH; }

	return std::string();

}


ResourceEnum ResourceManager::ConvertComponentTypeToResourceType(COMPONENT_TYPE* type) {

	switch (*type) {

	case COMPONENT_TYPE::CAMERA:

		return ResourceEnum::CAMERA;

	case COMPONENT_TYPE::MESH:

		return ResourceEnum::MESH;

	case COMPONENT_TYPE::MATERIAL:

		return ResourceEnum::MATERIAL;

	case COMPONENT_TYPE::ANIMATION:

		return ResourceEnum::ANIMATION;

	default:

		return ResourceEnum::UNKNOWN;
		break;

	}

	return ResourceEnum::UNKNOWN;

}


void ResourceManager::GetComponentIfLoaded(const ResourceEnum* type, Component** component, std::string fileName) {

	if (*type == ResourceEnum::MESH || *type == ResourceEnum::MATERIAL || *type == ResourceEnum::CAMERA) {	// Responsible to assign a loaded component to the selected material

		*component = App->editorScene->FindGameObjectByComponent(std::stoi(fileName));

		if (*component == nullptr) {

			GameObject* focus = App->editorScene->GetFocus();

			if (focus != nullptr) {

				if (*type == ResourceEnum::MESH) {

					if (focus->mesh != nullptr) { *component = focus->mesh; }
					else { *component = focus->AddComponent(COMPONENT_TYPE::MESH); }

				}

				else if (*type == ResourceEnum::MATERIAL) {

					if (focus->material != nullptr) { *component = focus->material; }
					else { *component = focus->AddComponent(COMPONENT_TYPE::MATERIAL); }

				}

				else if (*type == ResourceEnum::CAMERA) {

					if (focus->camera != nullptr) { *component = focus->camera; }
					else { *component = focus->AddComponent(COMPONENT_TYPE::CAMERA); }

				}

			}

		}

	}

}


