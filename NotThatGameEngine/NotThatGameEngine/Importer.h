#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include "OpenGLFuncionality.h"
#include "Application.h"

class GameObject;
class Component;
class Mesh;

namespace DataImporter {

	bool LoadScene(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);
	GameObject* LoadModel(Application* app, const char* path, const char* buffer = nullptr, uint size = 0, GameObject* parent = nullptr, bool enabled = true);
	void LoadMeshNode(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform);
	void LoadMeshMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId);
	uint LoadTexture(Application* app, const char* path, const char* buffer = nullptr, uint size = 0);

	void aiTransformTofloat4x4Transform(aiMatrix4x4 matrix, Transform* transform);
}

#endif
