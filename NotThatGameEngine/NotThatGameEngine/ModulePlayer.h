#ifndef __MODULEPLAYER__
#define __MODULEPlAYER__
#include "Module.h"


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

};

#endif