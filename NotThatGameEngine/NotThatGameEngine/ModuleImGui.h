#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

class ModuleImGui : public Module
{
public:
	ModuleImGui(Application* app, bool start_enabled = true);
	~ModuleImGui();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	float GetSliderDt();
	std::string AppName();

private:
	char* SDL;
	std::string GLEW;
	std::string ImGui;
	char* MathGeoLib;
	float sliderDt = 0.0f;
	char appName[30];

	update_status DefaultMenus(bool* demoMenu);
	void SetMainMenuBar(bool* demoMenu);
	void AboutMenu(bool* aboutMenu);
	void FPSMenu();

};

