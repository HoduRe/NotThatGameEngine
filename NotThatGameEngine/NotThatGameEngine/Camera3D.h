#ifndef __Camera3D_h__
#define __Camera3D_h__

#include "Module.h"
#include "glmath.h"
#include "Camera.h"

class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();
	void MoveCamera();

public:
	
	vec3 X, Y, Z, Position, Reference;	// XYZ are the camera axis (Z = camera forward, X = camera left, Y = camera up), position the camera position, reference where the camera is looking

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	float speed;
//	Camera camera;

};

#endif