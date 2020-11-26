#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"
#include "MathGeoLib/src/MathGeoLib.h"

class Camera : public Component {

public:

	Camera(long long int _id, GameObject* _gameObject);
	~Camera();

	void UpdateTransform();
	void SetFOV(float FOV);
	float GetFOV();
	void SetAspectRatio(float aspectRatio);
	void LookAt(float3 position);

public:

	Frustum camera;

private:

private:


};

#endif