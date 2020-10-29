#ifndef __ManagerImGui_h__
#define __ManagerImGui_h__

#include "Module.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

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

	std::string positionX;
	std::string positionY;
	std::string positionZ;
	std::string rotationX;
	std::string rotationY;
	std::string rotationZ;
	std::string scaleX;
	std::string scaleY;
	std::string scaleZ;

	update_status DefaultButtons();
	void SetMainMenuBar();
	void AboutMenu(bool* aboutMenu);
	update_status DefaultWindow();
	void ConsoleWindow();
	void SceneWindow();
	void HierarchyWindow();
	void InspectorWindow();

	void AddChildNode(GameObject* nextObject, int index);

};

#endif