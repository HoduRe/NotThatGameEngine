#ifndef __LOAD_H__
#define __LOAD_H__

#include "Globals.h"
#include "JsonManager.h"

class Component;
class Application;
class Mesh;
class Material;

namespace DataLoading {

	void LoadScene(Application* App, char* buffer);
	void LoadMesh(char* fileBuffer, Mesh* mesh);
	void LoadMaterial(Application* App, char* fileBuffer, Material* material);
	uint LoadTexture(Application* app, const char* path, const char* buffer = nullptr, uint size = 0);
	void LoadGameObject(Application* App, JSON_Array* gameObjectsArray, int index);

	struct ComponentReader {

		int componentType;
		long long int componentId;

	};

}

#endif