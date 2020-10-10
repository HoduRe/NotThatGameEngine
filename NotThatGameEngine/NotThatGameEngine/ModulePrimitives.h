#ifndef __MODULEPRIMITIVES_H__
#define __MODULEPRIMITIVES_H__

#include "Module.h"
#include "Globals.h"
#include "Primitives.h"

class ModulePrimitives : public Module
{

public:

	ModulePrimitives(Application* app, bool start_enabled = true);
	~ModulePrimitives();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

	CubeF cube;
	SphereF sphere;

private:

	void ExecuteEvent(EVENT_ENUM eventId);

};

#endif