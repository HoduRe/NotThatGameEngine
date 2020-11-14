#include "ManagerResource.h"
#include "parson/parson.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	json_set_allocation_functions();

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


void LoadScene() {

	// TODO: Load scene

}


void SaveScene() {

	// TODO: Save scene

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

		// TODO: Recive load / save event

	default:
		break;
	}

	return true;
}




