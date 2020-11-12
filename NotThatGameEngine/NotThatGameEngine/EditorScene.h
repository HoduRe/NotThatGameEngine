#ifndef __EDITORSCENE_H__
#define __EDITORSCENE_H__

#include "GameObject.h"
#include "Module.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/postprocess.h"

class EditorScene : public Module {

public:

	EditorScene(Application* app, bool start_enabled = true);
	~EditorScene();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool AddPrimitive(PrimitiveEnum _type);
	bool AddGameObject(GameObject* newObject);
	void DeleteGameObject(long long int id);
	void SetFocus(GameObject* gameobject = nullptr);
	GameObject* GetFocus();
	long long int GenerateId();

private:

	bool ExecuteEvent(EVENT_ENUM _event, void* var);
	void DeleteRootGameObjects();

public:

	std::vector<GameObject*> rootGameObjectsVec;
	bool sceneWindowFocus;

private:

	aiLogStream stream;
	uint defaultTextureId;
	GameObject* focus;

};

#endif

