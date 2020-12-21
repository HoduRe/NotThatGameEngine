#ifndef __LOAD_H__
#define __LOAD_H__

#include "Globals.h"
#include "JsonManager.h"
#include "ManagerResource.h"

class Component;
class Application;
class Mesh;
class Material;
class Camera;
class Animation;
class GameObject;

namespace DataLoading {

	void LoadScene(Application* App, char* buffer);
	void LoadGameObject(Application* App, JSON_Array* gameObjectsArray, int index);
	void LoadModel(Application* App, char* buffer);
	void LoadMesh(char* fileBuffer, Mesh* mesh);
	void LoadMaterial(Application* App, char* fileBuffer, Material* material);
	uint LoadTexture(Application* app, std::string path, const char* buffer = nullptr, uint size = 0);
	void LoadAnimation(char* fileBuffer, Animation* animation);
	void LoadCamera(char* fileBuffer, Camera* camera);

	void LoadAssetsMap(Application* App, std::map<std::string, FileInfo>* assetsMap);
	void LoadLibraryMap(Application* App, std::map<std::string, LibraryInfo>* libraryMap);

	struct ComponentReader {

		int componentType;
		long long int componentId;

	};

}

#endif