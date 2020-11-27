#ifndef __SAVE_H__
#define __SAVE_H__

#include "Globals.h"
#include "JsonManager.h"
#include "ManagerResource.h"

class Application;
class Component;
class Mesh;
class Material;
class GameObject;

namespace DataSaving {

	void SaveScene(Application* App);
	void RecursiveChildCall(Application* App, JSON_Array* gameObjectsArray, GameObject* gameObject);
	void SaveGameObject(Application* App, JSON_Object* node, GameObject* gameObject);
	void SaveComponentJSON(Application* App, JSON_Array* gameComponentsArray, Component* component, int arrayIndex);
	void SaveModel(Application* App, GameObject* gameObject, std::string modelName);
	void SaveMesh(Application* App, Mesh* mesh);
	void SaveMaterial(Application* App, Material* material);
	std::string SaveTexture(Application* App, std::string textureName);

	void SaveAssetsMap(Application* App, std::map<std::string, FileInfo>* assetsMap);
	void SaveLibraryMap(Application* App, std::map<std::string, LibraryInfo>* libraryMap);

}

#endif

