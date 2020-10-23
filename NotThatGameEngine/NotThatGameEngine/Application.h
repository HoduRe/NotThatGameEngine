#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "Window.h"
#include "Input.h"
#include "Audio.h"
#include "Scene.h"
#include "Renderer3D.h"
#include "Camera3D.h"
#include "Physics3D.h"
#include "Player.h"
#include "ResetModule.h"
#include "ManagerImGui.h"
#include "ManagerEvent.h"
#include "ManagerPrimitives.h"
#include "FileSystem.h"
#include "ManagerModel.h"

#define LOG(format, ...) App->log(__FILE__, __LINE__, format, __VA_ARGS__);

class Application
{
public:
	Window* window;
	Input* input;
	Audio* audio;
	Scene* scene_intro;
	Renderer3D* renderer3D;
	Camera3D* camera;
	Physics3D* physics;
	Player* player;
	ResetModule* reset;
	ManagerImGui* imGui;
	ManagerEvent* eventManager;
	ManagerPrimitives* primitives;
	FileSystem* fileLoad;
	ManagerModel* modelManager;

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

#endif