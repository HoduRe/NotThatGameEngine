#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include "Assimp/include/matrix4x4.h"

class Application;
class GameObject;
class Component;
class Mesh;
class Transform;
class aiNode;
class aiScene;

namespace Importer {

	std::string ImportNewModel(Application* app, const char* path, const char* buffer, uint size);
	bool ImportNewModelComponents(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);
	void ImportNewModelMesh(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform);
	void ImportNewModelMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId);
	void ImportAnimation(Application* App, aiScene* scene, GameObject* newObject);

	std::string ImportTexture(Application* App, std::string fileName, const char* buffer, uint size);

	void aiTransformTofloat4x4Transform(aiMatrix4x4 matrix, Transform* transform);
	void DeleteWithAllChilds(Application* App, GameObject* gameObject);

}

#endif
