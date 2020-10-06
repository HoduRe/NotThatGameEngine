#pragma once
#include "Module.h"
#include "Globals.h"

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


private:
	char* SDL;
	std::string GLEW;
	std::string ImGui;
	char* MathGeoLib;


	update_status DefaultMenus(bool* demoMenu);
	void SetMainMenuBar(bool* demoMenu);
	void AboutMenu(bool* aboutMenu);
	void FPSMenu();

};

