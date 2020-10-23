#ifndef __ManagerPrimitives_H__
#define __ManagerPrimitives_H__

#include "Module.h"
#include "Primitives.h"

class ManagerPrimitives : public Module
{

public:

	ManagerPrimitives(Application* app, bool start_enabled = true);
	~ManagerPrimitives();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

private:

	bool ExecuteEvent(EVENT_ENUM eventId, void* var);

private:
	std::vector<PrimitivesF> primitives;

};

#endif