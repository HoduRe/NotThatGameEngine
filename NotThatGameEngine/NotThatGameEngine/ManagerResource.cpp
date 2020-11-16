#include "Application.h"
#include "JsonManager.h"
#include "GameObject.h"
#include "SaveLoad.h"


ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	// TODO: Load scene in assets

	// TODO: Load all gameobjects using JSON

	// TODO: Load all gameobjects' components

	// TODO: Add components to gameobjects

	// TODO: Load other components (camera)

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


void ResourceManager::LoadScene() {

	// TODO: Load scene

}


void ResourceManager::SaveScene() {

	JSON_Value* root = json_value_init_object();
	JSON_Object* node = json_value_get_object(root);
	JSON_Array* gameObjectsArray = JsonManager::OpenArray(node, "GameObjects");

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;	// TODO: What about you get all gameobjects childs and all that jam :)

	for (uint i = 0; i < gameObjects.size(); i++) { DataSaving::SaveGameObject(App, JsonManager::AddArrayNode(gameObjectsArray), gameObjects[i]); }

	char* buffer = new char[JsonManager::GetArraySize(gameObjectsArray)];
	uint size = JsonManager::Serialize(root, &buffer);

	std::string sceneName = SCENES_PATH + (std::string)"Scene1.txt";
	App->externalManager->Save(sceneName.c_str(), buffer, size);

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		SaveScene();

		break;

	case EVENT_ENUM::LOAD_SCENE:

		LoadScene();

		break;

	default:
		break;
	}

	return true;
}




