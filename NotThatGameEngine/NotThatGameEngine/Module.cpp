#include "Module.h"

Module::Module(Application* parent, bool start_enabled) : App(parent), enabled(start_enabled){}


Module::~Module(){}


bool Module::Init()
{
	return true;
}


bool Module::Start()
{
	return true;
}


update_status Module::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}


update_status Module::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}


update_status Module::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}


bool Module::CleanUp()
{
	return true;
}


bool Module::Load()
{
	return true;
}


bool Module::Save()
{
	return true;
}


void Module::AddEvent(EVENT_ENUM eventId)
{

	listener.push_back(eventId);

}


void Module::RemoveEvent(EVENT_ENUM eventId)
{

	int numElem = listener.size();

	for (int i = 0; i < numElem; i++)
	{

		if (listener[i] == eventId)
		{
			listener.erase(listener.begin() + numElem);
		}

	}

}


bool Module::CheckListener(Module* mod)
{
	int numElem = listener.size();

	if (listener.size() == 0)
	{
		return false;
	}

	for (int i = 0; i < numElem; i++)
	{
		mod->ExecuteEvent(listener[i]);
		numElem = listener.size();
	}

	listener.clear();

	return true;
}


void Module::ExecuteEvent(EVENT_ENUM eventId)
{}


bool Module::IsEnabled() const { return enabled; }


void Module::Enable()
{

	if (enabled == false)
	{
		enabled = true;
		Start();
	}

}


void Module::Disable()
{

	if (enabled == true)
	{
		enabled = false;
		CleanUp();
	}

}


