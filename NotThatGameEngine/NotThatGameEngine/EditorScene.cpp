#include "EditorScene.h"


/*EditorScene::EditorScene() : root(nullptr)	// TODO make the tree structure. R_Scene actually just creates the first object of a gameobject hierarchy, but the vector of gameobjects is from Scene (this)
{
	root = new GameObject("Root Object", GenerateId());
}*/


EditorScene::EditorScene() : gameObjectIdCount(0), gameObjectVec() {}


EditorScene::~EditorScene() {
	int size = gameObjectVec.size();
	for (int i = size; i > -1; i--) {
		delete gameObjectVec[i];
		gameObjectVec[i] = nullptr;
	}
}


void EditorScene::Update(float dt)
{
	int size = gameObjectVec.size();
	for (int i = 0; i < size; i++) { gameObjectVec[i]->Update(); }
}


int EditorScene::GenerateId() { return gameObjectIdCount++; }


GameObject* EditorScene::AddGameObject(int id, std::string _name, GameObject* parent, bool enabled) {

	GameObject* newObject = new GameObject(id, _name, parent, enabled);
	gameObjectVec.push_back(newObject);
	return newObject;

}



