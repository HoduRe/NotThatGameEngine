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

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled), assetsMap(), libraryMap(), memoryMap() {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	PathNode assetsFiles = App->externalManager->GetAllFiles(ASSETS_PATH);
	PathNode libraryFiles = App->externalManager->GetAllFiles(LIBRARY_PATH);

	DataLoading::LoadAssetsMap(App, &assetsMap);
	DataLoading::LoadLibraryMap(App, &libraryMap);
	CheckLibraryFiles(&libraryFiles);
	CheckAssetsImported(&assetsFiles);
	CheckDeletedAssets();

	App->eventManager->EventRegister(EVENT_ENUM::SAVE_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_DROPPED, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_LOADING, this);
	App->eventManager->EventRegister(EVENT_ENUM::GAMEOBJECT_LOADED, this);

	return true;

}


bool ResourceManager::Start() {


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

	DataSaving::SaveAssetsMap(App, &assetsMap);
	DataSaving::SaveLibraryMap(App, &libraryMap);

	return true;

}


void ResourceManager::CheckLibraryFiles(PathNode* loadingNode) {

	for (int i = 0; i < loadingNode->children.size(); i++) {

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

	for (int i = 0; i < loadingNode->children.size(); i++) {

		if (loadingNode->children[i].isFile) { SearchFileInFileMap(loadingNode->children[i].path); }

		CheckAssetsImported(&loadingNode->children[i]);

	}

}


void ResourceManager::SearchFileInFileMap(std::string filePath) {

	std::string fileName, extension;
	App->externalManager->SplitFilePath(filePath.c_str(), nullptr, &fileName, &extension);

	if (assetsMap.count(fileName) == 1) {

		assetsMap[fileName].checked = true;
		if (assetsMap[fileName].lastTimeChanged) {/*Replace file on Library/ and update time*/ }
		// Remember scene has to have an update time, and if it's different, manageScene function is waiting for you :3

	}

	else { assetsMap.insert(std::pair<std::string, FileInfo>(fileName, FileInfo(filePath, App->idGenerator.Int(), 0/*TODO: Generate dates*/, true))); }

	if (libraryMap.count(fileName) == 0) { ImportToLibrary(filePath, fileName, extension); }

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

	for (std::map<std::string, FileInfo>::iterator it = assetsMap.begin(); it != assetsMap.end(); it++) {

		if (!assetsMap.at(it->first).checked) {

			std::map<std::string, FileInfo>::iterator itAux = it;
			if (it != assetsMap.begin()) { it--; }
			else { it++; }

			if (libraryMap.find(itAux->first)->second.type == ResourceEnum::SCENE) { ManageSceneFiles(std::string(), nullptr, libraryMap.find(itAux->first)->second.filePath); }
			App->externalManager->RemoveFileByName(libraryMap.find(itAux->first)->second.filePath.c_str());

			libraryMap.erase(libraryMap.find(itAux->first));
			assetsMap.erase(assetsMap.find(itAux->first));

		}

	}

}


void ResourceManager::LoadResourceByPath(std::string filePath) {

	char* buffer;
	uint size, id;
	std::string fileName, extension;
	ResourceEnum type = ResourceEnum::NONE;

	filePath = IsLoadedInLibrary(&filePath);
	App->externalManager->SplitFilePath(filePath.c_str(), nullptr, &fileName, &extension);
	type = GetTypeByExtension(extension);

	size = App->externalManager->Load(filePath.c_str(), &buffer);

	if (size != 0) {

		switch (type) {

		case ResourceEnum::EXTERNAL_MODEL:

			Importer::ImportNewModel(App, filePath.c_str(), buffer, size);
			// Add to libraryMap
			break;

		case ResourceEnum::OWN_MODEL:

			DataLoading::LoadModel(App, buffer);
			break;

		case ResourceEnum::TEXTURE:

			id = DataLoading::LoadTexture(App, filePath.c_str(), buffer, size);
			App->eventManager->GenerateEvent(EVENT_ENUM::PUT_TEXTURE_TO_FOCUSED_MODEL, EVENT_ENUM::NULL_EVENT, (void*)id);
			// If not in library, add to libraryMap
			break;

		case ResourceEnum::SCENE:

			DataLoading::LoadScene(App, buffer);
			break;

			// Components loaded from here are to add to an existing gameObject; at loading, it is the gameObject itself that does it for compatibility sake (LoadComponent())
		case ResourceEnum::MESH:

			DataLoading::LoadMesh(buffer, (Mesh*)App->editorScene->FindGameObjectByComponent(std::stoi(fileName))); // Those need work. I don't even know how to manage them
			break;

		case ResourceEnum::MATERIAL:

			DataLoading::LoadMaterial(App, buffer, (Material*)App->editorScene->FindGameObjectByComponent(std::stoi(fileName)));
			break;

		case ResourceEnum::CAMERA:

			DataLoading::LoadCamera(buffer, (Camera*)App->editorScene->FindGameObjectByComponent(std::stoi(fileName)));
			break;

		case ResourceEnum::UNKNOWN:
			break;

		default:
			break;
		}

	}


}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	std::string filePath, secondaryPath;
	char* buffer = nullptr;

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		filePath = DataSaving::SaveScene(App);
		App->externalManager->Load(filePath.c_str(), &buffer);
		App->externalManager->SplitFilePath(filePath.c_str(), nullptr, &secondaryPath);

		if (assetsMap.count(secondaryPath) == 1) {
			assetsMap.insert(std::pair<std::string, FileInfo>(secondaryPath, FileInfo(filePath, assetsMap.find(secondaryPath)->second.id, 0, true)));
		}	// Generate date :)
		else { assetsMap.insert(std::pair<std::string, FileInfo>(secondaryPath, FileInfo(filePath, App->idGenerator.Int(), 0, true))); }	// Generate date :)

		if (libraryMap.count(secondaryPath) == 1) { ManageSceneFiles(filePath, buffer, libraryMap.find(secondaryPath)->second.filePath); }
		else { ManageSceneFiles(filePath, buffer, std::string()); }

		break;

	case EVENT_ENUM::FILE_DROPPED:
	case EVENT_ENUM::FILE_LOADING:

		LoadResourceByPath((char*)var);
		LOG("File with path %s loaded.", (char*)var);

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

	// When a gameObject / Model is loaded, do things here with the Library and or Assets and Memory maps. gameObject is the root GameObject

	long long int id = App->idGenerator.Int();
	gameObject->id = id;

	if (gameObject->transform != nullptr) { gameObject->transform->id = App->idGenerator.Int(); }
	if (gameObject->mesh != nullptr) { gameObject->mesh->id = App->idGenerator.Int(); }
	if (gameObject->material != nullptr) { gameObject->material->id = App->idGenerator.Int(); }
	if (gameObject->camera != nullptr) { gameObject->camera->id = App->idGenerator.Int(); }

	for (int i = 0; i < gameObject->childs.size(); i++) { ManageGameObjectLoading(gameObject->childs[i]); }


}


std::string ResourceManager::ManageSceneFiles(std::string assetsScenePath, char* bufferAssets, std::string libraryScenePath) {

	std::string fileName, libraryAuxPath, componentPath;
	std::vector<int> assetsIDs, libraryIDs;

	if (assetsScenePath.empty()) { App->externalManager->SplitFilePath(libraryScenePath.c_str(), nullptr, &fileName); }	// When scene has been deleted from Assets/
	else { App->externalManager->SplitFilePath(assetsScenePath.c_str(), nullptr, &fileName); }

	libraryAuxPath = LIBRARY_SCENES_PATH + fileName + EXTENSION_SCENES;

	if (!libraryScenePath.empty()) {	// Enters when the saved scene has been re-saved: we check if there has been changes within, correct them, and delete deprecated components

		char* bufferLibrary;

		App->externalManager->Load(libraryAuxPath.c_str(), &bufferLibrary);
		assetsIDs = GetSceneComponents(bufferAssets);	// If there is no Scene, it 
		libraryIDs = GetSceneComponents(bufferLibrary);

		for (int i = 0; i < assetsIDs.size(); i += 2) {	// Not as redundant to re-loop the previously looped info as it seems

			for (int j = 0; j < libraryIDs.size(); j += 2) {

				if (assetsIDs[i] == libraryIDs[j]) {

					assetsIDs.erase(assetsIDs.begin() + (i + 1));
					assetsIDs.erase(assetsIDs.begin() + i);
					if (i != 0) { i -= 2; }
					libraryIDs.erase(libraryIDs.begin() + (j + 1));
					libraryIDs.erase(libraryIDs.begin() + j);
					j = libraryIDs.size();

				}

			}

		}

	}

	for (int i = 0; i < libraryIDs.size(); i += 2) {

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

	if (buffer != nullptr) {

		JsonManager::JsonValue root(json_parse_string(buffer));
		JSON_Object* node(json_value_get_object(root.value));
		JSON_Array* objectsNode(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));
		int size = JsonManager::GetArraySize(objectsNode);

		for (int i = 0; i < size; i++) {

			JSON_Object* nodeObject = json_array_get_object(objectsNode, i);
			JSON_Array* componentNode(json_object_get_array(nodeObject, JSON_NODE_COMPONENTS));
			int sizeComponents = JsonManager::GetArraySize(componentNode);

			for (int j = 0; j < sizeComponents; j++) {

				JSON_Object* nodeComponent = json_array_get_object(componentNode, j);
				componentIDs.push_back(json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_ID));
				componentIDs.push_back(json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_TYPE));

			}

		}

	}

	return componentIDs;

}


ResourceEnum ResourceManager::GetTypeByExtension(std::string extension) {

	extension = "." + extension;

	static_assert(static_cast<int>(ResourceEnum::UNKNOWN) == 8, "Code Needs Update");

	if (extension == EXTENSION_SCENES) { return ResourceEnum::SCENE; }
	else if (extension == EXTENSION_MODELS) { return ResourceEnum::OWN_MODEL; }
	else if (extension == EXTENSION_MESHES) { return ResourceEnum::MESH; }
	else if (extension == EXTENSION_MATERIALS) { return ResourceEnum::MATERIAL; }
	else if (extension == EXTENSION_CAMERA) { return ResourceEnum::CAMERA; }
	else if (extension == ".FBX" || extension == ".fbx" || extension == ".OBJ" || extension == ".obj") { return ResourceEnum::EXTERNAL_MODEL; }
	else if (extension == ".tga" || extension == ".png" || extension == ".jpg" || extension == ".dds" || extension == ".TGA"
		|| extension == ".PNG" || extension == ".JPG" || extension == ".DDS" || extension == EXTENSION_TEXTURES) {
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

	default:

		return ResourceEnum::UNKNOWN;
		break;

	}

	return ResourceEnum::UNKNOWN;

}


