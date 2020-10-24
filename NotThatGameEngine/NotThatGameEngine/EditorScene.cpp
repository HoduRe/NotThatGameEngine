#include "Application.h"

/*EditorScene::EditorScene() : root(nullptr)	// TODO make the tree structure. R_Scene actually just creates the first object of a gameobject hierarchy, but the vector of gameobjects is from Scene (this)
{
	root = new GameObject("Root Object", GenerateId());
}*/// TODO how do we deal with the parent-child conflict; How do we delete a gameobject without leaving it's child / parent node dirty	


EditorScene::EditorScene(Application* app, bool start_enabled) : Module(app, start_enabled), gameObjectIdCount(0), rootGameObjectsVec() {}


EditorScene::~EditorScene() {

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) {
		delete rootGameObjectsVec[i];
		rootGameObjectsVec[i] = nullptr;
	}
}


bool EditorScene::Init() { return true; }


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


int EditorScene::GenerateId() { return gameObjectIdCount++; }


GameObject* EditorScene::AddGameObject(int id, std::string _name, GameObject* parent, bool enabled) {

	GameObject* newObject = new GameObject(id, _name, parent, enabled);
	rootGameObjectsVec.push_back(newObject);
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
