#include "Application.h"
#include "ModulePrimitives.h"

ModulePrimitives::ModulePrimitives(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ModulePrimitives::~ModulePrimitives() {}


bool ModulePrimitives::Init() {

	bool ret = true;

	CubeF cube(3.0f);
	SphereF sphere(2.0f, 3, 3);
	PyramidF pyramid(5.0f, 2.0f);
	CylinderF cylinder(8.0f, 3.0f, 30);

	primitives.push_back(cube);
	primitives.push_back(sphere);
	primitives.push_back(pyramid);
	primitives.push_back(cylinder);

	return ret;
}


bool ModulePrimitives::CleanUp() {

	for (int i = 0; i < primitives.size(); i++) {
		glDeleteBuffers(1, (GLuint*)&primitives[i].idVertex);
		glDeleteBuffers(1, (GLuint*)&primitives[i].idIndex);
	}

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

	for (int i = 0; i < primitives.size(); i++) {
		primitives[i].BlitPrimitive();
	}

	return update_status::UPDATE_CONTINUE;
}


void ModulePrimitives::ExecuteEvent(EVENT_ENUM eventId) {


}

