#include "Application.h"

Scene::Scene(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

Scene::~Scene()
{}

// Load assets
bool Scene::Start()
{
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool Scene::CleanUp()
{
	return true;
}

// Update
update_status Scene::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;
}



