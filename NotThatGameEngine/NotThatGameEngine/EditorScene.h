#ifndef __EDITORSCENE_H__
#define __EDITORSCENE_H__

#include "Module.h"
#include "Primitives.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/postprocess.h"

class GameObject;
class Component;

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
	GameObject* FindGameObject(long long int id);
	void SetDeleteGameObject(long long int id);
	Component* FindGameObjectByComponent(long long int componentId);
	void SetFocus(GameObject* gameobject = nullptr);
	GameObject* GetFocus();
	void DeleteFromRootGameObjects(GameObject* gameobject, int index);
	void DeleteAllGameObjects();

private:

	bool ExecuteEvent(EVENT_ENUM _event, void* var);
	void DeleteLoop(GameObject* gameObject);

public:

	std::vector<GameObject*> rootGameObjectsVec;
	bool sceneWindowFocus;

private:

	aiLogStream stream;
	GameObject* focus;

};

#endif

