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

	return true;
}


update_status ResourceManager::PreUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::PostUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;

}


bool ResourceManager::CleanUp() {

	return true;

}


void ResourceManager::LoadScene() {

	// TODO: Load scene

}


void ResourceManager::SaveScene(char** buffer) {

	JsonManager file;
	JSON_Array* gameObjectsArray = file.OpenArray("GameObjects");

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;

	for (uint i = 1; i < gameObjects.size(); i++) { DataSaving::SaveGameObject(App, &file.AddArrayNode(gameObjectsArray), gameObjects[i]); }

	uint size = file.Serialize(buffer);

	// Now save this through file system :3

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

		// TODO: Recive load / save event

	default:
		break;
	}

	return true;
}




