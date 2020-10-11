#include "Application.h"
#include "ModulePrimitives.h"

ModulePrimitives::ModulePrimitives(Application* app, bool start_enabled) : Module(app, start_enabled), cube(), sphere() {}


ModulePrimitives::~ModulePrimitives() {}


bool ModulePrimitives::Init() {

	bool ret = true;

	std::vector<float> vertices = { 0.0f, 0.0f, 0.0f,   0.0f, 2.0f, 0.0f,   2.0f, 2.0f, 0.0f,   2.0f, 0.0f, 0.0f,   0.0f, 0.0f, 2.0f,   2.0f, 2.0f, 2.0f,   0.0f, 2.0f, 2.0f,   2.0f, 0.0f, 2.0f };


	std::vector<unsigned int> index = { 0, 1, 2,   3, 0, 2,	/* Front */ 4, 5, 6,   5, 4, 7,	// Back
								2, 1, 6,   5, 2, 6, /* Up */ 3, 4, 0,   7, 4, 3,	// Down
								4, 6, 0,   6, 1, 0,	/* Right */ 7, 3, 5,   5, 3, 2 };	// Left

	cube.SetAttributes(5);
//	cube.SetIndexVector(index);
//	sphere.SetAttributes(2.0f, 3, 3, 2.5f);

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

