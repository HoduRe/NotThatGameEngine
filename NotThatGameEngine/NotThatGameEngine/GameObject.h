#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include "MathGeoLib/src/MathBuildConfig.h"

class Component;
class Transform;
class Mesh;
class Material;
class Camera;
class Application;
enum class COMPONENT_TYPE;

class GameObject {

public:

	GameObject(long long int _id, std::string _name = "NewGameObject", GameObject* parent = nullptr, bool enabled = true, std::vector<GameObject*> children = std::vector<GameObject*>());
	~GameObject();

	void Update();
	void PostUpdate(Application* App);
	Component* AddComponent(COMPONENT_TYPE _type, long long int id = -1);
	bool AddGameObjectByParent(GameObject* newObject);
	bool CheckChildDeletionById(long long int _id);
	void SetDeleteGameObject(bool deleteBool = true);
	void SetDeleteComponent(COMPONENT_TYPE _type);
	Component* GetComponent(COMPONENT_TYPE _type);
	Component* FindGameObjectChildByComponent(long long int componentId);
	GameObject* FindGameObjectChild(long long int id);
	void CheckComponentDeletion();

public:

	std::string name;
	GameObject* parent;
	float4x4 worldTransform;
	std::vector<GameObject*> childs;

	long long int id;
	bool enabled;
	Mesh* mesh;	// Ugly code? Yes. More efficient code? Y E S
	Material* material;
	Camera* camera;
	Transform* transform;

	bool deleteGameObject;

private:

	void ManageAABB(Mesh* mesh);

private:

	LCG idGenerator;

};

#endif