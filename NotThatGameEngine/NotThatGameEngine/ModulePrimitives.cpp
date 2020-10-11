#include "Application.h"
#include "ModulePrimitives.h"

ModulePrimitives::ModulePrimitives(Application* app, bool start_enabled) : Module(app, start_enabled), cube(), sphere() {}


ModulePrimitives::~ModulePrimitives() {}


bool ModulePrimitives::Init() {

	bool ret = true;

	cube.SetAttributes(3);
	sphere.SetAttributes(2.0f, 3, 3);

	return ret;
}


bool ModulePrimitives::CleanUp() {

	return true;
}


update_status ModulePrimitives::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ModulePrimitives::Update(float dt) {

	CheckListener(this);

	return update_status::UPDATE_CONTINUE;
}


update_status ModulePrimitives::PostUpdate(float dt) {

	CheckListener(this);

	cube.BlitPrimitive();
	sphere.BlitPrimitive();

	return update_status::UPDATE_CONTINUE;
}


void ModulePrimitives::ExecuteEvent(EVENT_ENUM eventId) {


}

