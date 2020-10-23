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
	Component* AddComponent(COMPONENT_TYPE _type);
	int GenerateId();

public:

	std::string name;
	GameObject* parent;
	std::vector<GameObject*> childs;

	int id;
	bool enabled;
	std::vector<Component*> components;

private:

	int componentIdGenerator;

};

#endif