#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;

class Transform : public Component {

public:

	Transform(int _id, GameObject* _gameObject);
	~Transform();

	void Update();

public:

	float3 position;
	float3 rotationEuler;
	Quat rotation;
	float3 scale;

private:

	void RecalculateEulerAngles();

};

#endif