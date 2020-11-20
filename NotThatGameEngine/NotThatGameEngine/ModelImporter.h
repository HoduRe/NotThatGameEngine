#ifndef __MODEL_IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include "OpenGLFuncionality.h"
#include "Application.h"

class GameObject;
class Component;
class Mesh;

namespace ModelImporter {

	GameObject* LoadNewModel(Application* app, const char* path, const char* buffer = nullptr, uint size = 0, GameObject* parent = nullptr, bool enabled = true);
	bool LoadNewModelComponents(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);
	void LoadNewModelMesh(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform);
	void LoadNewModelMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId);

	void aiTransformTofloat4x4Transform(aiMatrix4x4 matrix, Transform* transform);

}

#endif
