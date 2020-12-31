#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include "MathGeoLib/src/Algorithm/Random/LCG.h"

class Module;
class Window;
class Input;
class Audio;
class Renderer3D;
class Camera3D;
class ManagerImGui;
class ManagerEvent;
class ExternalManager;
class ResourceManager;
class EditorScene;
class Texture;

#define LOG(format, ...) App->log(__FILE__, __LINE__, format, __VA_ARGS__);

class Application
{
public:
	Window* window;
	Input* input;
	Audio* audio;
	Renderer3D* renderer3D;
	Camera3D* camera;
	ManagerImGui* imGui;
	ManagerEvent* eventManager;
	ExternalManager* externalManager;
	ResourceManager* resourceManager;
	EditorScene* editorScene;
	Texture* texture;

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
	float GetDt();

	float framerate;
	std::vector<std::string> consoleVec;
	int consoleVecSize;

	// Data storage of framerate and ms
	std::vector<float> framerateVec;
	std::vector<float> msVec;
	int msVecCounter;
	int framerateVecCounter;
	LCG idGenerator;
	float gameDt;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

#endif