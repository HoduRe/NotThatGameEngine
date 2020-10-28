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

	void RecalculateTransformFromParent(float4x4& parentPosition);

	void SetEulerAngles(float3 eulerAngles);

public:

	float3 position;
	float3 rotationEuler;
	Quat rotation;
	float3 scale;
	float4x4 transform;

private:

	void RecalculateEulerAngles();
	void RecalculateTransform();

};

#endif