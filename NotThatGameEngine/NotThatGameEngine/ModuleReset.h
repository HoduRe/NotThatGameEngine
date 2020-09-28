#ifndef __ModuleReset_H__
#define __ModuleReset_H__

#include "Module.h"
#include "Globals.h"

class ModuleReset : public Module
{
public:
	ModuleReset(Application* app, bool start_enabled = true);
	~ModuleReset();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	bool reset;
};

#endif