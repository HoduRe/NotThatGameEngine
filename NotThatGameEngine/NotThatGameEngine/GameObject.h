#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Component.h"
#include "Transform.h"

class GameObject {

public:

	GameObject(int _id, std::string _name = "NewGameObject", GameObject* parent = nullptr, bool enabled = true);
	~GameObject();

	void Update();
	void PostUpdate();
	Component* AddComponent(COMPONENT_TYPE _type);
	int GenerateId();
	bool CheckChildDeletionById(int _id);
	void SetDeleteGameObject();

public:

	std::string name;
	GameObject* parent;
	std::vector<GameObject*> childs;

	int id;
	bool enabled;
	std::vector<Component*> components;

	bool deleteGameObject;

private:

	void CheckGameObjectDeletion();
	void CheckComponentDeletion();

private:

	int componentIdGenerator;

};

#endif