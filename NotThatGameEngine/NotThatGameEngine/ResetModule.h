#ifndef __ResetModule_H__
#define __ResetModule_H__

#include "Module.h"

class ResetModule : public Module
{
public:
	ResetModule(Application* app, bool start_enabled = true);
	~ResetModule();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void SetReset(bool state);

private:
	bool reset;
};

#endif