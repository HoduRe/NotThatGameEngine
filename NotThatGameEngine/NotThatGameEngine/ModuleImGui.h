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

	std::string AppName();
	float sliderDt;
	float sliderBrightness;
	int sliderWidth;
	int sliderHeight;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fullDesktop;

private:
	char* SDL;
	std::string GLEW;
	std::string ImGui;
	char* MathGeoLib;
	char appName[30];
	int refreshRate;
	bool AVX;
	bool AVX2;
	bool AltiVec;
	bool MMX;
	bool RDTSC;
	bool SSE;
	bool SSE2;
	bool SSE3;
	bool SSE41;
	bool SSE42;

	update_status DefaultButtons(bool* demoMenu);
	void SetMainMenuBar(bool* demoMenu);
	void AboutMenu(bool* aboutMenu);
	update_status DefaultWindow();
	void ConsoleWindow();
};

