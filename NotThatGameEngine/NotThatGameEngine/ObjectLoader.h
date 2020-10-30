#ifndef __OBJECTLOADER_H__
#define __OBJECTLOADER_H__

#include "Globals.h"
#include "OpenGLFuncionality.h"
#include "Application.h"

class GameObject;
class Application;
class Component;
class Mesh;

	GameObject* LoadModel(Application* app, const char* path, const char* buffer = nullptr, uint size = 0, GameObject* parent = nullptr, bool enabled = true);
	uint LoadTexture(Application* app, const char* path, const char* buffer = nullptr, uint size = 0);
	void LoadMeshNode(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform);

	bool LoadScene(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);

#endif
