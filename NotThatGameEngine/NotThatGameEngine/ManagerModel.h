#ifndef __ManagerModel_H__
#define __ManagerModel_H__

#include "Module.h"

#include "Mesh.h"

class Texture;

class ManagerModel : public Module
{

public:

	ManagerModel(Application* app, bool start_enabled = true);
	~ManagerModel();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadModel(const char* path);

public:

private:

	bool ExecuteEvent(EVENT_ENUM eventId, void* var);

private:
	Mesh testMesh;
};

#endif