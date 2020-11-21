#include "Application.h"

ResetModule::ResetModule(Application* app, bool start_enabled) : Module(app, start_enabled), reset(false) {}

ResetModule::~ResetModule(){}

bool ResetModule::Start() {
	reset = false;
	return true;
}

update_status ResetModule::Update(float dt) {	// IF YOU ARE TO USE THIS, CLEANUP AND START OF NECESSARY MODULES

	if (reset == true) {

		reset = false;
	}

	return update_status::UPDATE_CONTINUE;
}

bool ResetModule::CleanUp() { return true; }

void ResetModule::SetReset(bool state) { reset = state; }