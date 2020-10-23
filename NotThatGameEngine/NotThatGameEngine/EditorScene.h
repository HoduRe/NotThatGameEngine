#ifndef __EDITORSCENE_H__
#define __EDITORSCENE_H__

#include "GameObject.h"
#include "Module.h"

class EditorScene : public Module {

public:

	EditorScene(Application* app, bool start_enabled = true);
	~EditorScene();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);

	GameObject* AddGameObject(int id, std::string _name = "NewGameObject", GameObject* parent = nullptr, bool enabled = true);

private:

	int GenerateId();
	void ExecuteEvent(EVENT_ENUM _event);

public:

	std::vector<GameObject*> gameObjectVec;

private:

	int gameObjectIdCount;

};

#endif

