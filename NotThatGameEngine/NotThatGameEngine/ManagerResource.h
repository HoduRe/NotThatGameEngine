#ifndef __MANAGER_RESOURCE_H__
#define __MANAGER_RESOURCE_H__

#include "Module.h"

class ResourceManager : public Module {

public:

	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:

	bool ExecuteEvent(EVENT_ENUM _event, void* var);
	void LoadScene();
	void SaveScene(char** buffer);

public:



private:



};

#endif


