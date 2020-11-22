#ifndef __ManagerImGui_h__
#define __ManagerImGui_h__

#include "Module.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#define FILE_MAX_LENGTH 250

class GameObject;

class ManagerImGui : public Module
{
public:
	ManagerImGui(Application* app, bool start_enabled = true);
	~ManagerImGui();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadFileMenu(const char* directory = nullptr, const char* extension = nullptr);

	std::string AppName();
	float sliderDt;
	float sliderBrightness;
	int sliderWidth;
	int sliderHeight;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fullDesktop;
	bool vsync;

private:
	char* SDL;
	std::string GLEW;
	std::string ImGui;
	std::string Devil;
	std::string Assimp;
	std::string PhysFS;
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
	bool showDemoWindow;
	bool defaultButtonsMenu;
	bool aboutWindow;
	bool configMenu;
	bool appActive;
	bool consoleMenu;
	bool sceneWindow;
	bool hierarchyWindow;
	bool inspectorWindow;
	bool loadFileMenu;

	float3 position;
	float3 rotationEuler;
	float3 scaling;

	void DefaultButtons();
	update_status SetMainMenuBar();
	void AboutMenu();
	update_status DefaultWindow();
	void ConsoleWindow();
	void SceneWindow();
	void HierarchyWindow();
	void InspectorWindow();
	void DrawDirectoryRecursively(const char* directory, const char* extension);

	void AddChildNode(GameObject* nextObject, int index);

	char selectedFileName[FILE_MAX_LENGTH];

};

#endif