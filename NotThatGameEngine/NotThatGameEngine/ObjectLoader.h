#ifndef __OBJECTLOADER_H__
#define __OBJECTLOADER_H__

#include "Globals.h"
#include "OpenGLFuncionality.h"
#include "Application.h"

class GameObject;
class Application;
class Component;
class Mesh;

struct SceneObject {

	int meshID;
	aiMatrix4x4 transform;

};

	GameObject* LoadModel(Application* app, const char* path, const char* buffer = nullptr, uint size = 0, GameObject* parent = nullptr, bool enabled = true);
	uint LoadTexture(Application* app, const char* path, const char* buffer = nullptr, uint size = 0);
	void LoadMeshNode(aiNode* node, std::vector<SceneObject>& targetParent, aiMatrix4x4 accTransform);

	bool LoadScene(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);

#endif
