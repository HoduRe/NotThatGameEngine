#ifndef __EDITORSCENE_H__
#define __EDITORSCENE_H__

#include "GameObject.h"

class EditorScene {

public:

	EditorScene();
	~EditorScene();
	void Update(float dt);

	GameObject* AddGameObject(int id, std::string _name = "NewGameObject", GameObject* parent = nullptr, bool enabled = true);

private:

	int GenerateId();

public:

	std::vector<GameObject*> gameObjectVec;

private:

	int gameObjectIdCount;

};

#endif

