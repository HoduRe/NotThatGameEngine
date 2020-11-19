#include "Application.h"
#include "JsonManager.h"
#include "GameObject.h"
#include "SaveLoad.h"
#include "PathNode.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	LoadLibraryFiles();

	return true;

}


bool ResourceManager::Start()
{

	App->eventManager->EventRegister(EVENT_ENUM::SAVE_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::LOAD_SCENE, this);

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


// TODO: Move LoadScene and SaveScene to SaveLoad.h, or the new place to be saving things (SaveLoadComponents, SaveLoadX...), because if we implement models, we will have to have an intermedium between gameobject and scene
void ResourceManager::LoadLibraryFiles() {

	PathNode loadingFilesNode = App->externalManager->GetAllFiles(LIBRARY_PATH);

	std::vector<std::string> objectsVec = GetPathChildrenElements(loadingFilesNode);
	std::vector<std::string> sceneVec;
	std::vector<std::string> textureVec;
	std::vector<std::string> componentVec;

	for (int i = 0; i < objectsVec.size(); i++) {

		ResourceEnum type = CheckResourceType(objectsVec[i]);
		if (type == ResourceEnum::SCENE) { sceneVec.push_back(objectsVec[i]); }
		else if (type == ResourceEnum::TEXTURE) { textureVec.push_back(objectsVec[i]); }
		else if (type == ResourceEnum::MATERIAL || type == ResourceEnum::MESH) { componentVec.push_back(objectsVec[i]); }

	}

	if (sceneVec.size() != 0) { LoadResourceByType(sceneVec[0], ResourceEnum::SCENE); }	// We only want to load one scene
	else {

		LOG("No scene to be loaded.\n");
		return;

	}

	for (int i = 0; i < textureVec.size(); i++) { LoadResourceByType(textureVec[i], ResourceEnum::TEXTURE); }

	for (int i = 0; i < objectsVec.size(); i++) { LoadResourceByType(objectsVec[i]); }

	// TODO: Load other components (camera)


	// TODO: Load other objects which are not "saved", in the Assets/Library folder


	objectsVec.clear();
	sceneVec.clear();
	textureVec.clear();

}


void ResourceManager::LoadScene(char* buffer) {

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));

	int size = JsonManager::GetArraySize(gameObjectsArray);

	for (int i = 0; i < size; i++) { DataLoading::LoadGameObject(App, gameObjectsArray, i); }

}


void ResourceManager::SaveScene() {

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(JsonManager::OpenArray(node, JSON_NODE_GAMEOBJECTS));

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;

	for (int i = 0; i < gameObjects.size(); i++) {

		for (int j = 0; j < gameObjects[i]->childs.size(); j++) { gameObjects.push_back(gameObjects[i]->childs[j]); } // TODO: This should be called recursively :)

		DataSaving::SaveGameObject(App, JsonManager::AddArrayNode(gameObjectsArray), gameObjects[i]);

	}

	// TODO: Save cameras, lights...

	char* buffer = new char[JsonManager::GetArraySize(gameObjectsArray)];
	uint size = JsonManager::Serialize(root.value, &buffer);

	std::string sceneName = LIBRARY_PATH + (std::string)"Scene1" + EXTENSION_SCENES;
	App->externalManager->Save(sceneName.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}


std::vector<std::string> ResourceManager::GetPathChildrenElements(PathNode loadingNode) {

	std::vector<std::string> objectsVec;

	for (int i = 0; i < loadingNode.children.size(); i++) {

		if (loadingNode.children[i].isFile) { objectsVec.push_back(loadingNode.children[i].path); }
		else {

			std::vector<std::string> newVector = GetPathChildrenElements(loadingNode.children[i]);
			objectsVec.insert(objectsVec.end(), newVector.begin(), newVector.end());

		}

	}

	return objectsVec;

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		SaveScene();

		break;

	case EVENT_ENUM::LOAD_SCENE:

		break;

	case EVENT_ENUM::SAVE_ENGINE:



		break;

	case EVENT_ENUM::LOAD_ENGINE:

		LoadLibraryFiles();

		break;

	default:
		break;
	}

	return true;
}


ResourceEnum ResourceManager::CheckResourceType(std::string name) {

	std::string ext;

	App->externalManager->SplitFilePath(name.c_str(), nullptr, nullptr, &ext);

	ext = "." + ext;

	static_assert(static_cast<int>(ResourceEnum::UNKNOWN) == 6, "Code Needs Update");

	if (ext == EXTENSION_SCENES) { return ResourceEnum::SCENE; }
	else if (ext == EXTENSION_MODELS) { return ResourceEnum::MODEL; }
	else if (ext == EXTENSION_MESHES) { return ResourceEnum::MESH; }
	else if (ext == EXTENSION_MATERIALS) { return ResourceEnum::MATERIAL; }
	else if (ext == ".FBX" || ext == ".fbx" || ext == ".OBJ" || ext == ".obj") { return ResourceEnum::MODEL; }
	else if (ext == ".tga" || ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".TGA" || ext == ".PNG" || ext == ".JPG" || ext == ".DDS" || ext == EXTENSION_TEXTURES) { return ResourceEnum::TEXTURE; }

	return ResourceEnum::UNKNOWN;

}


void ResourceManager::LoadResourceByType(std::string name, ResourceEnum type) {

	ResourceEnum resourceType;
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	char* buffer = nullptr;
	std::string fileName;

	type == ResourceEnum::NONE ? resourceType = CheckResourceType(name) : resourceType = type;

	switch (resourceType) {

	case ResourceEnum::SCENE:

		App->externalManager->Load(name.c_str(), &buffer);
		LoadScene(buffer);

		break;

	case ResourceEnum::MODEL:



		break;

	case ResourceEnum::MESH:

		App->externalManager->SplitFilePath(name.c_str(), nullptr, &fileName);
		mesh = (Mesh*)App->editorScene->FindGameObjectByComponent((long long int)std::stoi(fileName));

		if (mesh != nullptr) {

			App->externalManager->Load(name.c_str(), &buffer);
			DataLoading::LoadMesh(buffer, mesh);

		}

		break;

	case ResourceEnum::MATERIAL:

		App->externalManager->SplitFilePath(name.c_str(), nullptr, &fileName);
		material = (Material*)App->editorScene->FindGameObjectByComponent((long long int)std::stoi(fileName));

		if (material != nullptr) {

			App->externalManager->Load(name.c_str(), &buffer);
			DataLoading::LoadMaterial(App, buffer, material);

		}

		break;

	case ResourceEnum::TEXTURE:

		DataLoading::LoadTexture(App, name.c_str());

		break;

	default:
		break;
	}

	RELEASE_ARRAY(buffer);

}


