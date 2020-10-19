#pragma once

#include <vector>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModulePlayer.h"
#include "ModuleReset.h"
#include "ModuleImGui.h"
#include "ModuleEventManager.h"
#include "ModulePrimitives.h"
#include "ModuleFileLoad.h"
#include "ModelManager.h"

#define LOG(format, ...) App->log(__FILE__, __LINE__, format, __VA_ARGS__);

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModulePlayer* player;
	ModuleReset* reset;
	ModuleImGui* imGui;
	ModuleEventManager* eventManager;
	ModulePrimitives* primitives;
	ModuleFileLoad* fileLoad;
	ModelManager* modelManager;

	Timer ms_timer;

private:

	float dt;
	bool userDt;	// User defines the dt
	std::vector <Module*> modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void UserDt(bool dt);
	void log(const char file[], int line, const char* format, ...);

	float framerate;
	std::vector<std::string> consoleVec;
	int consoleVecSize;

	// Data storage of framerate and ms
	std::vector<float> framerateVec;
	std::vector<float> msVec;
	int msVecCounter;
	int framerateVecCounter;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

