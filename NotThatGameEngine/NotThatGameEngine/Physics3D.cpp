#include "Physics3D.h"

Physics3D::Physics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
Physics3D::~Physics3D()
{}

// Render not available yet----------------------------------
bool Physics3D::Init()
{
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool Physics3D::Start()
{
	return true;
}

// ---------------------------------------------------------
update_status Physics3D::PreUpdate(float dt)
{


	return update_status::UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status Physics3D::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status Physics3D::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool Physics3D::CleanUp()
{
	return true;
}

