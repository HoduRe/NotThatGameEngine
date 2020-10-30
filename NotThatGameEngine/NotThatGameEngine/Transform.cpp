#include "Transform.h"
#include "GameObject.h"

#if _DEBUG
#pragma comment ( lib, "MathGeoLib/libx86/Debug/MathGeoLib.lib" )

#else

#pragma comment ( lib, "MathGeoLib/libx86/Release/MathGeoLib.lib" )

#endif


Transform::Transform(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::TRANSFORM), position(0.0f, 0.0f, 0.0f),
rotationEuler(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(Quat::identity) { RecalculateTransform(); }


Transform::~Transform() {}


void Transform::Update() {

	RecalculateEulerAngles();

}


void Transform::RecalculateTransformFromParent(float4x4& parentTransform) {

	owner->worldTransform = parentTransform * transform;
	transform.Decompose(position, rotation, scale);
	RecalculateEulerAngles();

}


void Transform::RecalculateEulerAngles() { rotationEuler = rotation.ToEulerXYZ() * RADTODEG; }


void Transform::RecalculateTransform() {

	transform = float4x4::FromTRS(position, rotation, scale);
	RecalculateEulerAngles();

}


void Transform::SetEulerAngles(float3 eulerAngles) {

	float3 newEluerRotation = (eulerAngles - rotationEuler) * DEGTORAD;
	Quat newRotation = Quat::FromEulerXYZ(newEluerRotation.x, newEluerRotation.y, newEluerRotation.z);
	rotation = rotation * newRotation;
	rotationEuler = newEluerRotation;
	RecalculateTransform();

}




