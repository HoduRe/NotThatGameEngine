#include "ManagerPrimitives.h"

ManagerPrimitives::ManagerPrimitives(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ManagerPrimitives::~ManagerPrimitives() {}


bool ManagerPrimitives::Init() {

	bool ret = true;

	primitives.push_back(CubeF(3.0f));
	primitives.push_back(SphereF(2.0f, 36, 18));
	primitives.push_back(PyramidF(5.0f, 2.0f));
	primitives.push_back(CylinderF(8.0f, 3.0f, 30));

	return ret;
}


bool ManagerPrimitives::CleanUp() {

	for (int i = 0; i < primitives.size(); i++) {
		glDeleteBuffers(1, (GLuint*)&primitives[i].idVertex);
		glDeleteBuffers(1, (GLuint*)&primitives[i].idIndex);
	}

	return true;
}


update_status ManagerPrimitives::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ManagerPrimitives::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ManagerPrimitives::PostUpdate(float dt) {

	for (int i = 0; i < primitives.size(); i++) {
		primitives[i].BlitPrimitive();
	}

	return update_status::UPDATE_CONTINUE;
}


bool ManagerPrimitives::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	return false;

}

