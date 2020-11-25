#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Camera : public Component {

public:

	Camera(long long int _id, GameObject* _gameObject);
	~Camera();

	void Update();
	void PostUpdate();

public:

	Frustum camera;

private:

private:


};

#endif