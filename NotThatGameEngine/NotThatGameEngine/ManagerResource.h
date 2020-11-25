#ifndef __MANAGER_RESOURCE_H__
#define __MANAGER_RESOURCE_H__

#include "Module.h"
#include "PathNode.h"

#define JSON_NODE_GAMEOBJECTS "GameObjects"
#define JSON_NODE_NAME "Name"
#define JSON_NODE_ID "ID"
#define JSON_NODE_PARENT_ID "ParentID"
#define JSON_NODE_TRANSLATION "Translation"
#define JSON_NODE_ROTATION "Rotation"
#define JSON_NODE_SCALE "Scale"
#define JSON_NODE_ENABLED "Enabled"
#define JSON_NODE_COMPONENTS "Components"
#define JSON_NODE_COMPONENT_TYPE "ComponentType"
#define JSON_NODE_COMPONENT_ID "ComponentID"

#define EXTENSION_SCENES ".NotThatScene"
#define EXTENSION_MODELS ".NotThatModel"
#define EXTENSION_MESHES ".NotThatMesh"
#define EXTENSION_MATERIALS ".NotThatMaterial"
#define EXTENSION_TEXTURES ".dds"

enum class ResourceEnum {

	NONE,

	TEXTURE,
	EXTERNAL_MODEL,
	OWN_MODEL,
	MESH,
	MATERIAL,
	SCENE,

	UNKNOWN

};

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
	void ImportAssetsFile(PathNode* child);
	bool IsLoadedInLibrary(std::string* filePath, ResourceEnum* type);
	void LoadResourceByType(std::string name, ResourceEnum type = ResourceEnum::NONE);
	void ImportAssetResourceByType(std::string name, ResourceEnum type = ResourceEnum::NONE);
	std::string FindPathFromFileName(std::string fileName, PathNode* node = nullptr);
	ResourceEnum CheckResourceType(std::string name, std::string* extension, std::string* fileName = nullptr);

private:

	std::vector<std::string> GetPathChildrenElements(PathNode loadingNode);
	std::string GetPathFromType(ResourceEnum type);
	std::string ConvertLoadExtension(ResourceEnum type, std::string extension);
	bool ExecuteEvent(EVENT_ENUM _event, void* var);

public:

	PathNode assetsFiles;

private:

};

#endif


