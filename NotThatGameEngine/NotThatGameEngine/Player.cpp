#include "Player.h"

Player::Player(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

Player::~Player()
{}

// Load assets
bool Player::Start()
{
	return true;
}

// Unload assets
bool Player::CleanUp()
{

	return true;
}

// Update: draw background
update_status Player::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;
}

