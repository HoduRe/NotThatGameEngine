#include "Transform.h"
#include "GameObject.h"

#if _DEBUG
#pragma comment ( lib, "MathGeoLib/libx86/Debug/MathGeoLib.lib" )

#else

#pragma comment ( lib, "MathGeoLib/libx86/Release/MathGeoLib.lib" )

#endif


Transform::Transform(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::TRANSFORM), localPosition(0.0f, 0.0f, 0.0f),
rotationEuler(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f, 1.0f) {}


Transform::~Transform() {}


void Transform::Update() {

	RecalculateEulerAngles();

}


void Transform::RecalculateTransform(float3& parentPosition) {



}


void Transform::RecalculateEulerAngles() { rotationEuler = rotation.ToEulerXYZ(); }



