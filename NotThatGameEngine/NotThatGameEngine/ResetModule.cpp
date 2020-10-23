#include "Application.h"

ResetModule::ResetModule(Application* app, bool start_enabled) : Module(app, start_enabled), reset(false) {}

ResetModule::~ResetModule(){}

bool ResetModule::Start() {
	reset = false;
	return true;
}

update_status ResetModule::Update(float dt) {	// IF YOU ARE TO USE THIS, UPDATE WITH THE NEW MODULES
	if (reset == true) {
		App->player->CleanUp();
		App->scene_intro->CleanUp();
		App->physics->CleanUp();
		App->physics->Start();
		App->scene_intro->Start();
		App->player->Start();
		reset = false;
	}
	return update_status::UPDATE_CONTINUE;
}

bool ResetModule::CleanUp() { return true; }

void ResetModule::SetReset(bool state) { reset = state; }