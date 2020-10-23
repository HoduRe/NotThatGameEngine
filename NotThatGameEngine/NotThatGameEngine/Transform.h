#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

class GameObject;

class Transform : public Component {

public:

	Transform(int _id, GameObject* _gameObject);
	~Transform();

};

#endif