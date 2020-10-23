#include "Application.h"

/*EditorScene::EditorScene() : root(nullptr)	// TODO make the tree structure. R_Scene actually just creates the first object of a gameobject hierarchy, but the vector of gameobjects is from Scene (this)
{
	root = new GameObject("Root Object", GenerateId());
}*/


EditorScene::EditorScene(Application* app, bool start_enabled) : Module(app, start_enabled), gameObjectIdCount(0), gameObjectVec() {}


EditorScene::~EditorScene() {
	int size = gameObjectVec.size();
	for (int i = size; i > -1; i--) {
		delete gameObjectVec[i];
		gameObjectVec[i] = nullptr;
	}
}


bool EditorScene::Init() {

	App->eventManager->EventRegister(EVENT_ENUM::DELETE_GAMEOBJECT_COMPONENT, this);

	return true;
}


update_status EditorScene::PreUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


update_status EditorScene::Update(float dt)
{
	int size = gameObjectVec.size();
	for (int i = 0; i < size; i++) { gameObjectVec[i]->Update(); }

	return update_status::UPDATE_CONTINUE;
}


int EditorScene::GenerateId() { return gameObjectIdCount++; }


GameObject* EditorScene::AddGameObject(int id, std::string _name, GameObject* parent, bool enabled) {

	GameObject* newObject = new GameObject(id, _name, parent, enabled);
	gameObjectVec.push_back(newObject);
	return newObject;

}


bool EditorScene::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

	case EVENT_ENUM::DELETE_GAMEOBJECT_COMPONENT:

		break;

	default:
		break;
	}

	return false;
}
