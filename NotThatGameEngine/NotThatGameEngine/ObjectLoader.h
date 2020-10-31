#ifndef __OBJECTLOADER_H__
#define __OBJECTLOADER_H__

#include "Globals.h"
#include "OpenGLFuncionality.h"
#include "Application.h"

class GameObject;
class Application;
class Component;
class Mesh;

bool LoadScene(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);
GameObject* LoadModel(Application* app, const char* path, const char* buffer = nullptr, uint size = 0, GameObject* parent = nullptr, bool enabled = true);
void LoadMeshNode(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform);
void LoadMeshMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId);
uint LoadTexture(Application* app, const char* path, const char* buffer = nullptr, uint size = 0);

void aiTransformTofloat4x4Transform(aiMatrix4x4 matrix, Transform* transform);

#endif
