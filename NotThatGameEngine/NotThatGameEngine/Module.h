#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"

class Application;

class Module
{
public:
	Application* App;

	Module(Application* parent, bool start_enabled = true);

	virtual ~Module();

	virtual bool Init();

	virtual bool Start();

	virtual update_status PreUpdate(float dt);

	virtual update_status Update(float dt);

	virtual update_status PostUpdate(float dt);

	virtual bool CleanUp();

	void AddEvent(EVENT_ENUM eventId);

	void RemoveEvent(EVENT_ENUM eventId);

	bool CheckListener(Module* mod);

	virtual bool ExecuteEvent(EVENT_ENUM eventId, void* var = nullptr);

	// Module activation
	bool IsEnabled() const;

	void Enable();

	void Disable();

private:
	std::vector<EVENT_ENUM>	listener;
	bool enabled;
};

#endif __MODULE_H__