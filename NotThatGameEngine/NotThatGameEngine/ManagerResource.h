#ifndef __MANAGER_RESOURCE_H__
#define __MANAGER_RESOURCE_H__

#include "Module.h"
#include "PathNode.h"
#include <map>

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

#define JSON_NODE_MAP_DATA "MapData"
#define JSON_NODE_MAP_FILENAME "FileName"
#define JSON_NODE_MAP_FILEPATH "FilePath"
#define JSON_NODE_MAP_ID "ID"
#define JSON_NODE_MAP_LAST_CHANGE "LastTimeChanged"
#define JSON_NODE_MAP_TYPE "Type"

#define EXTENSION_SCENES ".NotThatScene"
#define EXTENSION_MODELS ".NotThatModel"
#define EXTENSION_CAMERA ".NotThatCamera"
#define EXTENSION_MESHES ".NotThatMesh"
#define EXTENSION_MATERIALS ".NotThatMaterial"
#define EXTENSION_TEXTURES ".dds"
#define EXTENSION_MAP ".NotThatMap"

#define ASSETS_MAP "AssetsMap"
#define LIBRARY_MAP "LibraryMap"

enum class ResourceEnum {

	NONE,

	CAMERA,
	EXTERNAL_MODEL,
	MESH,
	MATERIAL,
	OWN_MODEL,
	SCENE,
	TEXTURE,

	UNKNOWN

};

class FileInfo {
public:
	FileInfo();
	FileInfo(std::string _filePath, int _id, int _lastChange);
	~FileInfo();
	std::string filePath;
	int id;
	int lastTimeChanged;
	bool checked;
};


class LibraryInfo {
public:
	LibraryInfo();
	LibraryInfo(std::string _filePath, ResourceEnum _type);
	~LibraryInfo();
	std::string filePath;
	ResourceEnum type;
};


class MemoryInfo {
public:
	MemoryInfo();
	~MemoryInfo();
	std::string libraryPath;
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
	bool IsLoadedInLibrary(std::string* filePath, ResourceEnum* type);
	void LoadResourceByType(std::string name, ResourceEnum type = ResourceEnum::NONE);
	void ImportAssetResourceByType(std::string path, std::string name, ResourceEnum type);
	std::string FindPathFromFileName(std::string fileName, PathNode* node = nullptr);
	ResourceEnum CheckTypeByExtension(std::string extension);

private:

	void CheckLibraryFiles(PathNode* libraryFiles);
	void CheckAssetsImported(PathNode* loadingNode);
	void ImportToLibrary(std::string filePath, std::string fileName, std::string extension);
	void CheckDeletedAssets();

	std::string GetPathFromType(ResourceEnum type);
	std::string ConvertLoadExtension(ResourceEnum type, std::string extension);
	bool ExecuteEvent(EVENT_ENUM _event, void* var);
	void SearchFileInFileMap(std::string filePath);

public:

private:

	std::map<std::string, FileInfo> assetsMap;	// Key is fileName
	std::map<std::string, LibraryInfo> libraryMap;	// Key is fileName
	std::map<int, MemoryInfo> memoryMap;	// Key is id

};

#endif


