#pragma once
#include "Module.h"

class Scene : public Module
{
public:
	Scene(Application* app, bool start_enabled = true);
	~Scene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

};
