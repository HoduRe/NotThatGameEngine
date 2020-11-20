#ifndef __SAVE_H__
#define __SAVE_H__

#include "Globals.h"
#include "JsonManager.h"

class Component;
class Application;
class Mesh;
class Material;
class GameObject;

namespace DataSaving {

	void SaveScene(Application* App);
	void SaveMesh(Application* App, Mesh* mesh);
	void SaveMaterial(Application* App, Material* material);
	std::string SaveTexture(Application* App, std::string textureName);
	void SaveGameObject(Application* App, JSON_Object* node, GameObject* gameObject);
	void SaveComponent(Application* App, Component* component);

}

#endif

