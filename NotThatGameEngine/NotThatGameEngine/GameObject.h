#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Component.h"
#include "Transform.h"

class GameObject {

public:

	GameObject();
	~GameObject();

	std::string name;
	GameObject* parent;
	std::vector<GameObject*> childs;

	int id;
	bool enabled;
	Transform* transform;
	std::vector<Component*> components;

};

#endif