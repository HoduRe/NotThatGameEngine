#ifndef __Physics3D_h__
#define __Physics3D_h__
#pragma once
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Physics3D : public Module
{
public:
	Physics3D(Application* app, bool start_enabled = true);
	~Physics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	
private:

};

#endif