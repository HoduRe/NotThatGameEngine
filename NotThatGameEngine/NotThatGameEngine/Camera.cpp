#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"

Camera::Camera(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::CAMERA), camera() {	// CAMERA COMPONENT MACHINE BROKEN. UNDERSTANDABLE HAVE A GOOD DAY

/*	camera.SetKind(FrustumSpaceGL, FrustumRightHanded);
	camera.SetPos(float3(0, 0, 0));
	camera.SetFront(float3(0, 0, 1));
	camera.SetUp(float3(0, 1, 0));

	camera.SetViewPlaneDistances(0.1f, 1000.0f);
	camera.SetPerspective(1.0f, 1.0f);
	*/
}


Camera::~Camera() {}


void Camera::UpdateTransform() {

/*	float4x4 transform = owner->worldTransform;
	float3 position, scale;
	float4x4 rotation;
	camera.SetFront(transform.WorldZ());
	camera.SetUp(transform.WorldY());
	transform.Decompose(position, rotation, scale);
	camera.SetPos(position);*/

}


float Camera::GetFOV() { /*return camera.VerticalFov() * RADTODEG;*/ return 0.0f; }


void Camera::SetFOV(float FOV) { /*camera.SetVerticalFovAndAspectRatio(FOV * RADTODEG, camera.AspectRatio());*/ }


void Camera::SetAspectRatio(float aspectRatio) { /*camera.SetHorizontalFovAndAspectRatio(camera.HorizontalFov(), aspectRatio);*/ }


void Camera::LookAt(float3 position) {

	/*float3 lookDirection = position - camera.Pos();
	float3x3 lookMatrix = float3x3::LookAt(camera.Front(), lookDirection.Normalized(), camera.Up(), float3(0, 1, 0));

	camera.SetFront(lookMatrix.MulDir(camera.Front()).Normalized());
	camera.SetUp(lookMatrix.MulDir(camera.Up()).Normalized());
	*/
}


