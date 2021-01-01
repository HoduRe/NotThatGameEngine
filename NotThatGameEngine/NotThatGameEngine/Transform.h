#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject;

class Transform : public Component {

public:

	Transform(long long int _id, GameObject* _gameObject);
	~Transform();

	void RecalculateTransformFromParent(float4x4& parentPosition);

	void SetPosition(float3 position);
	void SetRotation(Quat _rotation);
	void SetEulerAngles(float3 eulerAngles);
	void SetScale(float3 scale);

	float3 GetPosition();
	Quat GetEulerQuat();
	float3 GetEulerAngles();
	float3 GetScale();

	void RecalculateEulerAngles();

public:

	float4x4 transform;

private:

	void RecalculateTransform();

private:

	float3 position;
	float3 rotationEuler;
	Quat rotation;
	float3 scale;

};

#endif