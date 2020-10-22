#include "ModulePlayer.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;
}

