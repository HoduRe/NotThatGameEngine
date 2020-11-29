#include "Transform.h"
#include "GameObject.h"

Transform::Transform(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::TRANSFORM), position(0.0f, 0.0f, 0.0f),
rotationEuler(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(Quat::identity) {
	RecalculateTransform();
}


Transform::~Transform() {}


void Transform::RecalculateTransformFromParent(float4x4& parentTransform) {

	owner->worldTransform = parentTransform * transform;
	transform.Decompose(position, rotation, scale);
	RecalculateEulerAngles();

}


void Transform::RecalculateEulerAngles() { rotationEuler = rotation.ToEulerXYZ() * RADTODEG; }


void Transform::RecalculateTransform() { transform = float4x4::FromTRS(position, rotation, scale); RecalculateEulerAngles(); }


void Transform::SetPosition(float3 _position) {

	position = _position;
	RecalculateTransform();

}


void Transform::SetRotation(Quat _rotation) {

	rotation = _rotation;
	RecalculateTransform();

}


void Transform::SetEulerAngles(float3 eulerAngles) {

	float3 deviation = (eulerAngles - rotationEuler) * DEGTORAD;
	Quat newRotation = Quat::FromEulerXYZ(deviation.x, deviation.y, deviation.z);
	rotation = rotation * newRotation;
	rotationEuler = eulerAngles;
	RecalculateTransform();

}


void Transform::SetScale(float3 _scale) {

	if (_scale.x == 0) { _scale.x = 0.001; }
	if (_scale.y == 0) { _scale.y = 0.001; }
	if (_scale.z == 0) { _scale.z = 0.001; }
	scale = _scale;
	RecalculateTransform();

}


float3 Transform::GetPosition() { return position; }


Quat Transform::GetEulerQuat() { return rotation; }


float3 Transform::GetEulerAngles() { return rotationEuler; }


float3 Transform::GetScale() { return scale; }




