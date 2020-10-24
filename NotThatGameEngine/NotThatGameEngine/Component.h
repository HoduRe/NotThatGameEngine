#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class GameObject;

enum class COMPONENT_TYPE {
	NONE,

	TRANSFORM,
	MESH,

	ERROR_TYPE
};

class Component {

public:

	Component(int _id, GameObject* _gameObject, COMPONENT_TYPE _type, bool _enabled = true);
	~Component();

	virtual void Enable();
	virtual void Disable();
	virtual void Update();

	void SetComponentDelete();

public:

	int id;
	bool enabled;
	bool deleteComponent;
	COMPONENT_TYPE type;
	GameObject* owner;

};

#endif