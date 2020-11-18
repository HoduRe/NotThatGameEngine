#ifndef __MANAGER_RESOURCE_H__
#define __MANAGER_RESOURCE_H__

#include "Module.h"

#define JSON_NODE_GAMEOBJECTS "GameObjects"
#define JSON_NODE_NAME "Name"
#define JSON_NODE_ID "ID"
#define JSON_NODE_PARENT_ID "ParentID"
#define JSON_NODE_TRANSLATION "Translation"
#define JSON_NODE_ROTATION "Rotation"
#define JSON_NODE_SCALE "Scale"
#define JSON_NODE_COMPONENTS "Components"
#define JSON_NODE_COMPONENT_TYPE "ComponentType"
#define JSON_NODE_COMPONENT_ID "ComponentID"

#define EXTENSION_SCENE ".NotThatScene"

class PathNode;

class ResourceManager : public Module {


public:

	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadLibraryFiles();
	void LoadScene(char* buffer);
	void SaveScene();

private:

	std::vector<std::string> GetPathChildrenElements(PathNode loadingNode);
	bool ExecuteEvent(EVENT_ENUM _event, void* var);

public:



private:

	struct ComponentReader {

		int componentType;
		long long int componentId;

	};

};

#endif


