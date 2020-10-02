#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	player = new ModulePlayer(this);
	reset = new ModuleReset(this);
	imGui = new ModuleImGui(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(reset);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(player);

	// Renderer last!
	AddModule(renderer3D);
	AddModule(imGui);
}

Application::~Application()
{
	modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	int size = modules.size();

	for(int i = 0; i < size; i++)
	{
		modules[i]->Init();
	}

	for (int i = 0; i < size; i++)
	{
		modules[i]->Start();
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;
	PrepareUpdate();
	
	int size = modules.size();

	for (int i = 0; i < size && ret == update_status::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->PreUpdate(dt);
		if (ret != update_status::UPDATE_CONTINUE) { i = size; }
	}

	for (int i = 0; i < size && ret == update_status::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->Update(dt);
		if (ret != update_status::UPDATE_CONTINUE) { i = size; }
	}

	for (int i = 0; i < size && ret == update_status::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->PostUpdate(dt);
		if (ret != update_status::UPDATE_CONTINUE) { i = size; }
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	int size = modules.size();

	for (int i = size-1; i > -1; i--)
	{
		ret = modules[i]->CleanUp();
		if (ret == false) { i = -1; }
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}