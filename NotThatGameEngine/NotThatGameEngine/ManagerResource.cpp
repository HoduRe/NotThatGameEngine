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

	JsonManager file;
	JSON_Array* gameObjectsArray = file.OpenArray("GameObjects");

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;

	for (uint i = 1; i < gameObjects.size(); i++) { DataSaving::SaveGameObject(App, &file.AddArrayNode(gameObjectsArray), gameObjects[i]); }

	char* buffer = new char[file.GetArraySize(gameObjectsArray)];
	uint size = file.Serialize(&buffer);

	std::string sceneName = SCENES_PATH + (std::string)"Scene1.json";
	App->externalManager->Save(sceneName.c_str(), &buffer, size);

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




