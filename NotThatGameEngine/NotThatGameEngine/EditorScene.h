#ifndef __EDITORSCENE_H__
#define __EDITORSCENE_H__

#include "GameObject.h"
#include "Module.h"
#include "OpenGLFuncionality.h"

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

	bool AddGameObject(GameObject* newObject);
	GameObject* AddGameObjectByLoadingModel(const char* path, const char* objectName = nullptr, GameObject* parent = nullptr, bool enabled = true);
	void DeleteGameObject(int id);

private:

	int GenerateId();
	bool ExecuteEvent(EVENT_ENUM _event, void* var);
	void DeleteRootGameObjects();

public:

	std::vector<GameObject*> rootGameObjectsVec;

private:

	int gameObjectIdCount;	// TODO: remember to save this in the save file
	aiLogStream stream;
	uint defaultTextureId;

};

#endif

