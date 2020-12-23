#include "Application.h"
#include "ManagerImGui.h"
#include "Window.h"
#include "Renderer3D.h"
#include "Camera3D.h"
#include "ManagerEvent.h"
#include "Audio.h"
#include "EditorScene.h"
#include "Textures.h"
#include "TextureData.h"
#include "ManagerExternal.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Animation.h"
#include "Input.h"

#include "Save.h"
#include "Assimp/include/version.h"
#include "PhysFS/include/physfs.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


#include <shellapi.h>

ManagerImGui::ManagerImGui(Application* app, bool start_enabled) : Module(app, start_enabled), SDL((char*)""), MathGeoLib((char*)""), sliderDt(0.0f), appName("NotThatGameEngine"),
sliderBrightness(1.0f), sliderWidth(SCREEN_WIDTH* SCREEN_SIZE), sliderHeight(SCREEN_HEIGHT* SCREEN_SIZE), vsync(true),
fullscreen(WIN_FULLSCREEN), resizable(WIN_RESIZABLE), borderless(WIN_BORDERLESS), fullDesktop(WIN_FULLSCREEN_DESKTOP), refreshRate(0),
AVX(false), AVX2(false), AltiVec(false), MMX(false), RDTSC(false), SSE(false), SSE2(false), SSE3(false), SSE41(false), SSE42(false),
showDemoWindow(false), defaultButtonsMenu(false), aboutWindow(false), configMenu(false), appActive(false), consoleMenu(true), sceneWindow(true), hierarchyWindow(true), inspectorWindow(true),
Devil(), Assimp(), PhysFS(), GLEW(), loadFileMenu(false), selectedFilePath(), position(), rotationEuler(), scaling(), itemHovered(nullptr), itemFocusedLastFrame(nullptr), loadMeshMenu(false),
deletedFileName(), dragDropFile(), loadTexturesMenu(false), hierarchyWindowPos(), hierarchyWindowSize(), hasHierarchyFocus(false), referenceMenu(true), gameMode(false), playWindow(true)
{}


ManagerImGui::~ManagerImGui()
{}


bool ManagerImGui::Init()
{
	bool ret = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");


	LOG("ImGui initialized.\n");

	// Dynamically showing the version of the libs
	GLEW = "GLEW ";
	char* glewVersion = (char*)glewGetString(GLEW_VERSION);
	strcat_s((char*)GLEW.c_str(), GLEW.size() + 1 + strlen(glewVersion), glewVersion);

	ImGui = "ImGui ";
	const char* preImGuivers = ImGui::GetVersion();
	strcat_s((char*)ImGui.c_str(), ImGui.size() + 1 + strlen(preImGuivers), preImGuivers);

	SDL = (char*)"SDL 2.0.12";
	MathGeoLib = (char*)"MathGeoLib 1.5";

	int version = ilGetInteger(IL_VERSION_NUM);
	Devil = "DevIl " + std::to_string(version / 100);
	version = version % 100;
	Devil = Devil + "." + std::to_string(version / 10);
	version = version % 10;
	Devil = Devil + "." + std::to_string(version);

	PHYSFS_Version version2;
	PHYSFS_VERSION(&version2);

	Assimp = "ASSIMP " + std::to_string(aiGetVersionMajor()) + "." + std::to_string(aiGetVersionMinor());
	PhysFS = "PhysFS " + std::to_string(version2.major) + "." + std::to_string(version2.minor);

	//Get refresh rate
	SDL_DisplayMode displayMode;
	int numDisplayModes = SDL_GetNumDisplayModes(0);
	SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(App->window->window), SDL_GetWindowDisplayMode(App->window->window, &displayMode), &displayMode);
	refreshRate = displayMode.refresh_rate;

	AVX = SDL_HasAVX();
	AVX2 = SDL_HasAVX2();
	AltiVec = SDL_HasAltiVec();
	MMX = SDL_HasMMX();
	RDTSC = SDL_HasRDTSC();
	SSE = SDL_HasSSE();
	SSE2 = SDL_HasSSE2();
	SSE3 = SDL_HasSSE3();
	SSE41 = SDL_HasSSE41();
	SSE42 = SDL_HasSSE42();

	return ret;
}


bool ManagerImGui::Start()
{
	return true;
}


update_status ManagerImGui::PreUpdate(float dt) {

	ImVec4 clear_color = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	return update_status::UPDATE_CONTINUE;

}


update_status ManagerImGui::Update(float dt) {

	update_status ret = update_status::UPDATE_CONTINUE;
	update_status ret2 = update_status::UPDATE_CONTINUE;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGuiDockNodeFlags flag = ImGuiDockNodeFlags_NoDockingInCentralNode;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), flag);

	SceneWindow();

	if (gameMode == false) {

		if (showDemoWindow) { ImGui::ShowDemoWindow(&showDemoWindow); }	// DEMO WINDOW
		DefaultButtons();
		ret = SetMainMenuBar();
		ret2 = DefaultWindow();
		AboutMenu();
		ConsoleWindow();
		HierarchyWindow();
		InspectorWindow();
		ReferenceWindow();
		if (loadFileMenu) { LoadFileMenu(ASSETS_PATH, nullptr); }

	}

	ImGui::EndFrame();

	if (ret == update_status::UPDATE_CONTINUE && ret2 == update_status::UPDATE_CONTINUE) { return ret; }
	else { return update_status::UPDATE_STOP; }
}


update_status ManagerImGui::PostUpdate(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);	// Swaps current window with the other used by OpenGL (by default it uses double-buffered contexts)

	HierarchyManagement();

	return ret;
}


bool ManagerImGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}


update_status ManagerImGui::SetMainMenuBar() {

	update_status ret = update_status::UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar()) {	// TODO: order those in different tabs

		if (ImGui::BeginMenu("File")) {

			if (ImGui::MenuItem("New")) {}

			if (ImGui::MenuItem("Open")) {}

			if (ImGui::MenuItem("Save Scene")) { App->eventManager->GenerateEvent(EVENT_ENUM::SAVE_SCENE); }

			if (ImGui::MenuItem("Load Asset")) {

				loadFileMenu = true;
				selectedFilePath.clear();

			}

			if (ImGui::MenuItem("Close App")) { ret = update_status::UPDATE_STOP; }

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Edit")) {

			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}

			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}

			ImGui::Separator();

			if (ImGui::MenuItem("Cut", "CTRL+X")) {}

			if (ImGui::MenuItem("Copy", "CTRL+C")) {}

			if (ImGui::MenuItem("Paste", "CTRL+V")) {}

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Create Primitive")) {

			if (ImGui::MenuItem("Cube")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_CUBE); }

			if (ImGui::MenuItem("Sphere")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_SPHERE); }

			if (ImGui::MenuItem("Pyramid")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_PYRAMID); }

			if (ImGui::MenuItem("Cylinder")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_CYLINDER); }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Editor Windows")) {

			if (ImGui::MenuItem("Scene Window")) { sceneWindow = !sceneWindow; }

			if (ImGui::MenuItem("Hierarchy Window")) { hierarchyWindow = !hierarchyWindow; }

			if (ImGui::MenuItem("Inspector Window")) { inspectorWindow = !inspectorWindow; }

			if (ImGui::MenuItem("Referencing Window")) { referenceMenu = !referenceMenu; }

			if (ImGui::MenuItem("Console output")) { consoleMenu = !consoleMenu; }

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Configuration")) {

			if (ImGui::MenuItem("App options")) { configMenu = !configMenu; }

			if (ImGui::MenuItem("OpenGL options")) { defaultButtonsMenu = !defaultButtonsMenu; }

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Dev Info")) {

			if (ImGui::MenuItem("Documentation")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/wiki", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Download latest")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/releases", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Report a bug")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/issues", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Gui Demo")) { showDemoWindow = !showDemoWindow; }

			if (ImGui::MenuItem("About")) { aboutWindow = !aboutWindow; }

			ImGui::EndMenu();

		}

		ImGui::EndMainMenuBar();

	}

	return ret;
}


void ManagerImGui::AboutMenu() {

	if (aboutWindow) {

		ImGui::Begin("About", &aboutWindow);
		ImGui::Text("NotThatGameEngine\n\nAn attempt to create a usable game engine >:3\nBy Ferran-Roger Basart i Bosch:");
		if (ImGui::Button("GitHub")) { ShellExecuteA(0, "open", "https://github.com/ferba93/", NULL, NULL, SW_SHOWNORMAL); }
		ImGui::Text("\n\nLIBRARIES:");
		if (ImGui::Button(SDL)) { ShellExecuteA(0, "open", "https://www.libsdl.org/", NULL, NULL, SW_SHOWNORMAL); }
		if (ImGui::Button(GLEW.c_str())) { ShellExecuteA(0, "open", "http://glew.sourceforge.net/index.html", NULL, NULL, SW_SHOWNORMAL); }
		if (ImGui::Button(ImGui.c_str())) { ShellExecuteA(0, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL); }
		if (ImGui::Button(MathGeoLib)) { ShellExecuteA(0, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL); }
		if (ImGui::Button(Assimp.c_str())) { ShellExecuteA(0, "open", "https://www.assimp.org/", NULL, NULL, SW_SHOWNORMAL); }
		if (ImGui::Button(PhysFS.c_str())) { ShellExecuteA(0, "open", "https://icculus.org/physfs/", NULL, NULL, SW_SHOWNORMAL); }
		if (ImGui::Button(Devil.c_str())) { ShellExecuteA(0, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL); }
		ImGui::Text("\n\nLICENSE:\n\nMIT License\n\nCopyright(c) 2020 Ferran - Roger Basart i Bosch\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this softwareand associated documentation files(the 'Software'), \nto deal in the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell\ncopies of the Software, and to permit persons to whom the Software is furnished\nto do so, subject to the following conditions : \n");
		ImGui::Text("\tThe above copyright noticeand this permission notice shall be included in all\n\tcopies or substantial portions of the Software.\n");
		ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\nINCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
		ImGui::End();

	}
}


update_status ManagerImGui::DefaultWindow() {

	update_status ret = update_status::UPDATE_CONTINUE;
	static char organization[25] = "UPC CITM";
	char title[25];

	if (configMenu) {

		ImGui::Begin("Configuration", &configMenu);

		if (ImGui::CollapsingHeader("Application")) {

			if (ImGui::InputText("App name", appName, IM_ARRAYSIZE(appName))) { App->window->SetTitle(appName); }

			ImGui::InputText("Organization", organization, IM_ARRAYSIZE(organization));

			if (ImGui::SliderFloat("Max FPS", &sliderDt, 0.0f, 80.0f, "%.0f")) {

				if (sliderDt == 0) { App->UserDt(false); }
				else { App->UserDt(true); }

			}

			ImGui::Text("Limited frames: %.0f", App->framerate);

			//ImGui::Text();	// Limited frames
			sprintf_s(title, 25, "Framerate %.1f", App->framerateVec.back());
			ImGui::PlotHistogram("##framerate", &App->framerateVec[0], App->framerateVecCounter, 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Miliseconds %0.1f", App->msVec.back());
			ImGui::PlotHistogram("##miliseconds", &App->msVec[0], App->msVecCounter, 0, title, 0.0f, 40.0f, ImVec2(310, 100));

			if (ImGui::Checkbox("Vsync", &vsync)) { SDL_GL_SetSwapInterval(vsync); }
		}

		if (ImGui::CollapsingHeader("Window")) {
			if (ImGui::Checkbox("Active", &appActive)) { if (appActive == false) { ret = update_status::UPDATE_STOP; } }
			if (ImGui::SliderFloat("Brightness", &sliderBrightness, 0.0f, 1.0f)) { App->eventManager->GenerateEvent(EVENT_ENUM::SCREEN_BRIGHTNESS); }
			if (ImGui::SliderInt("Width", &sliderWidth, 0, 1980)) { App->eventManager->GenerateEvent(EVENT_ENUM::CHANGE_WINDOW_WIDTH, EVENT_ENUM::NULL_EVENT, &sliderWidth); }
			if (ImGui::SliderInt("Height", &sliderHeight, 0, 1280)) { App->eventManager->GenerateEvent(EVENT_ENUM::CHANGE_WINDOW_HEIGHT); }
			ImGui::Text("Refresh rate: %i", refreshRate);
			if (ImGui::Checkbox("Fullscreen", &fullscreen)) { App->eventManager->GenerateEvent(EVENT_ENUM::FULLSCREEN); }
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable)) { App->eventManager->GenerateEvent(EVENT_ENUM::RESIZABLE_WINDOW); }
			if (ImGui::Checkbox("Borderless", &borderless)) { App->eventManager->GenerateEvent(EVENT_ENUM::BORDERLESS_WINDOW); }
			ImGui::SameLine();
			if (ImGui::Checkbox("Full desktop", &fullDesktop)) { App->eventManager->GenerateEvent(EVENT_ENUM::FULLDESKTOP_WINDOW); }
		}

		if (ImGui::CollapsingHeader("Software & hardware")) {
			ImGui::Text(SDL);
			ImGui::Text(GLEW.c_str());
			ImGui::Text(ImGui.c_str());
			ImGui::Text(MathGeoLib);
			ImGui::Text(Assimp.c_str());
			ImGui::Text(PhysFS.c_str());
			ImGui::Text(Devil.c_str());
			ImGui::NewLine();
			ImGui::Text("CPUs: %i (Cache: %i kb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
			ImGui::Text("System RAM: %.2f Gb)", (float)SDL_GetSystemRAM() * 0.0009765625);	// The long number is 1 / 1024, because function returns in Mb
			if (AVX) { ImGui::Text("AVX  "); } ImGui::SameLine();
			if (AVX2) { ImGui::Text("AVX2  "); } ImGui::SameLine();
			if (AltiVec) { ImGui::Text("AltiVec  "); } ImGui::SameLine();
			if (MMX) { ImGui::Text("MMX  "); } ImGui::SameLine();
			if (RDTSC) { ImGui::Text("RDTSC  "); }
			if (SSE) { ImGui::Text("SSE  "); } ImGui::SameLine();
			if (SSE2) { ImGui::Text("SSE2  "); } ImGui::SameLine();
			if (SSE3) { ImGui::Text("SSE3  "); } ImGui::SameLine();
			if (SSE41) { ImGui::Text("SSE41  "); } ImGui::SameLine();
			if (SSE42) { ImGui::Text("SSE42  "); }
		}

		if (ImGui::CollapsingHeader("Audio")) {
			ImGui::SliderInt("SFX volume", &App->audio->sfxAdjustment, 0, 255);
			ImGui::SliderInt("Music volume", &App->audio->musicAdjustment, 0, 255);
		}

		ImGui::End();
	}

	return ret;
}


void ManagerImGui::DefaultButtons()
{
	static bool wireframe = false;

	if (defaultButtonsMenu) {

		ImGui::Begin("OpenGL Options", &defaultButtonsMenu);

		if (ImGui::Button("Depth test")) {
			if (glIsEnabled(GL_DEPTH_TEST)) { glDisable(GL_DEPTH_TEST); }
			else { glEnable(GL_DEPTH_TEST); }
		}

		if (ImGui::Button("Cull face")) {
			if (glIsEnabled(GL_CULL_FACE)) { glDisable(GL_CULL_FACE); }
			else { glEnable(GL_CULL_FACE); }
		}

		if (ImGui::Button("Lighting")) {
			if (glIsEnabled(GL_LIGHTING)) { glDisable(GL_LIGHTING); }
			else { glEnable(GL_LIGHTING); }
		}

		if (ImGui::Button("Color material")) {
			if (glIsEnabled(GL_COLOR_MATERIAL)) { glDisable(GL_COLOR_MATERIAL); }
			else { glEnable(GL_COLOR_MATERIAL); }
		}

		if (ImGui::Button("Texture")) {
			if (glIsEnabled(GL_TEXTURE_2D)) { glDisable(GL_TEXTURE_2D); }
			else { glEnable(GL_TEXTURE_2D); }
		}

		if (ImGui::Button("Fog")) {
			if (!glIsEnabled(GL_FOG)) { glEnable(GL_FOG); }
			else { glDisable(GL_FOG); }
		}

		if (ImGui::Button("Limited colors")) {
			if (glIsEnabled(GL_MINMAX)) { glDisable(GL_MINMAX); }
			else { glEnable(GL_MINMAX); }
		}

		if (ImGui::Button("Wireframe mode")) {
			wireframe = !wireframe;
			if (wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
			else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
		}

		ImGui::End();
	}

}


void ManagerImGui::ConsoleWindow() {

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysHorizontalScrollbar;

	if (consoleMenu) {

		ImGui::Begin("Console", &consoleMenu, flags);
		for (int i = 0; i < App->consoleVecSize; i++) { ImGui::Text(App->consoleVec[i].c_str()); }
		ImGui::End();

	}
}


void ManagerImGui::SceneWindow() {

	bool ret = false;

	if (sceneWindow) {

		ImGuiWindowFlags flag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		ImGuiWindowFlags flag2 = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

		if (!gameMode) {

			ImGui::Begin("Scene", &sceneWindow, flag);

			ret = ImGui::IsWindowFocused();

			ImVec2 vec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			ImVec2 uvMin = ImVec2(0.0f, 1.0f);
			ImVec2 uvMax = ImVec2(1.0f, 0.0f);

			ImGui::Image((ImTextureID*)App->renderer3D->sceneTextureId, vec2, uvMin, uvMax);

			ImGui::End();

		}

		else {

			ImGui::SetWindowPos("Game Scene", ImVec2(0, 0));
			ImGui::Begin("Game Scene", &sceneWindow, flag2);

			ret = ImGui::IsWindowFocused();

			ImVec2 vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
			ImVec2 uvMin = ImVec2(0.0f, 1.0f);
			ImVec2 uvMax = ImVec2(1.0f, 0.0f);

			ImGui::Image((ImTextureID*)App->renderer3D->sceneTextureId, vec2, uvMin, uvMax);

			ImGui::End();

		}

		ImVec2 size(160, 40);

		ImGui::SetWindowPos("Play Window", ImVec2((SCREEN_WIDTH * 0.5) - size.x * 0.5, SCREEN_HEIGHT * 0.05));
		ImGui::SetWindowSize("Play Window", size);
		
		if (gameMode) { ImGui::SetNextWindowFocus(); }

		ImGui::Begin("Play Window", &playWindow, flag2);

		if (!gameMode) {

			if (ImGui::Button("Play", ImVec2(40, 20))) {

				gameMode = true;
				App->eventManager->GenerateEvent(EVENT_ENUM::ENTERING_GAME_MODE);

			}

		}

		else {

			if (ImGui::Button("Stop", ImVec2(40, 20))) {

				gameMode = false;
				App->eventManager->GenerateEvent(EVENT_ENUM::LEAVING_GAME_MODE);

			}

			ImGui::SameLine();

			ImGui::Button("Pause", ImVec2(40, 20));
			ImGui::SameLine();

			ImGui::Button("Tick", ImVec2(40, 20));

		}

		ImGui::End();

	}

	App->editorScene->sceneWindowFocus = ret;

}


void ManagerImGui::HierarchyWindow() {

	itemHovered = nullptr;

	if (hierarchyWindow) {

		int size = App->editorScene->rootGameObjectsVec.size();

		ImGui::Begin("Scene Objects", &hierarchyWindow);
		hierarchyWindowPos = ImGui::GetWindowPos();
		hierarchyWindowSize = ImGui::GetWindowSize();
		hasHierarchyFocus = ImGui::IsWindowFocused();

		ImGui::Button("CreateGO");
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && ImGui::IsItemClicked()) {

			if (App->editorScene->GetFocus() == nullptr) { App->editorScene->AddGameObject(new GameObject(App, App->idGenerator.Int())); }
			else { App->editorScene->AddGameObject(new GameObject(App, App->idGenerator.Int(), "NewGameObject", App->editorScene->GetFocus())); }

		}

		ImGui::SameLine();

		ImGui::Button("DeleteGO");
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && ImGui::IsItemClicked()) {
			if (App->editorScene->GetFocus() != nullptr) { App->editorScene->SetDeleteGameObject(App->editorScene->GetFocus()->id); }
		}

		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && MouseIsInside(hierarchyWindowPos, hierarchyWindowSize) && hasHierarchyFocus) { App->editorScene->SetFocus(nullptr); }

		for (int i = 0; i < size; i++) {

			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool node_open;
			int childSize = App->editorScene->rootGameObjectsVec[i]->childs.size();

			if (App->editorScene->GetFocus() != nullptr) {
				if (App->editorScene->GetFocus()->id == App->editorScene->rootGameObjectsVec[i]->id) { node_flags |= ImGuiTreeNodeFlags_Selected; }
			}

			if (childSize != 0) {

				node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, App->editorScene->rootGameObjectsVec[i]->name.c_str(), i);
				if (ImGui::IsItemClicked()) { App->editorScene->SetFocus(App->editorScene->rootGameObjectsVec[i]); }
				if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {

					itemHovered = App->editorScene->rootGameObjectsVec[i];
					ImGui::BeginTooltip();
					ImGui::Text("Hovering on %s.", (App->editorScene->rootGameObjectsVec[i]->name + std::to_string(App->editorScene->rootGameObjectsVec[i]->id)).c_str());
					ImGui::EndTooltip();

				}

				if (node_open) {

					for (int j = 0; j < childSize; j++) { AddChildNode(App->editorScene->rootGameObjectsVec[i]->childs[j], j); }
					ImGui::TreePop();

				}

			}

			else {

				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, App->editorScene->rootGameObjectsVec[i]->name.c_str(), i);
				if (ImGui::IsItemClicked()) { App->editorScene->SetFocus(App->editorScene->rootGameObjectsVec[i]); }
				if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {

					itemHovered = App->editorScene->rootGameObjectsVec[i];
					ImGui::BeginTooltip();
					ImGui::Text("Hovering on %s.", (App->editorScene->rootGameObjectsVec[i]->name + std::to_string(App->editorScene->rootGameObjectsVec[i]->id)).c_str());
					ImGui::EndTooltip();

				}

			}

		}

		ImGui::End();
	}

}


void ManagerImGui::AddChildNode(GameObject* nextObject, int index) {

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open;
	int childSize = nextObject->childs.size();

	if (App->editorScene->GetFocus() != nullptr) {
		if (App->editorScene->GetFocus()->id == nextObject->id) { node_flags |= ImGuiTreeNodeFlags_Selected; }
	}

	if (childSize != 0) {

		node_open = ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, nextObject->name.c_str(), index);
		if (ImGui::IsItemClicked()) { App->editorScene->SetFocus(nextObject); }
		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {

			itemHovered = nextObject;
			ImGui::BeginTooltip();
			ImGui::Text("Hovering on %s.", (nextObject->name + std::to_string(nextObject->id)).c_str());
			ImGui::EndTooltip();

		}

		if (node_open) {

			for (int j = 0; j < childSize; j++) { AddChildNode(nextObject->childs[j], j); }
			ImGui::TreePop();

		}

	}

	else {

		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		node_open = ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, nextObject->name.c_str(), index);
		if (ImGui::IsItemClicked()) { App->editorScene->SetFocus(nextObject); }
		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {

			itemHovered = nextObject;
			ImGui::BeginTooltip();
			ImGui::Text("Hovering on %s.", (nextObject->name + std::to_string(nextObject->id)).c_str());
			ImGui::EndTooltip();

		}

	}

}


void ManagerImGui::InspectorWindow() {

	if (inspectorWindow) {

		ImGui::Begin("Inspector", &inspectorWindow);

		GameObject* focus = App->editorScene->GetFocus();

		if (focus != nullptr) {

			ImGui::Text((focus->name + std::to_string(focus->id)).c_str());

			Transform* transform = focus->transform;
			Material* material = focus->material;
			Mesh* mesh = focus->mesh;
			Animation* animation = focus->animation;
			Camera* camera = focus->camera;
			TextureData* textureData = nullptr;

			if (material != nullptr) { if (App->texture->IsTextureRepeated(material->GetTextureName()) != 0) { textureData = App->texture->GetTextureData(material->GetTextureName()); } }

			position = transform->GetPosition();
			rotationEuler = transform->GetEulerAngles();
			scaling = transform->GetScale();

			if (ImGui::CollapsingHeader("Transform")) {

				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll;

				ImGui::Text("Position:");
				if (ImGui::InputFloat("Position X", &position.x, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetPosition(position); }
				if (ImGui::InputFloat("Position Y", &position.y, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetPosition(position); }
				if (ImGui::InputFloat("Position Z", &position.z, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetPosition(position); }

				ImGui::Text("Rotation:");
				if (ImGui::InputFloat("Rotation X", &rotationEuler.x, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetEulerAngles(rotationEuler); }
				if (ImGui::InputFloat("Rotation Y", &rotationEuler.y, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetEulerAngles(rotationEuler); }
				if (ImGui::InputFloat("Rotation Z", &rotationEuler.z, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetEulerAngles(rotationEuler); }

				ImGui::Text("Scale:");
				if (ImGui::InputFloat("Scale X", &scaling.x, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetScale(scaling); }
				if (ImGui::InputFloat("Scale Y", &scaling.y, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetScale(scaling); }
				if (ImGui::InputFloat("Scale Z", &scaling.z, 0.0f, 0.0f, "%.4f", input_text_flags)) { transform->SetScale(scaling); }

			}

			if (ImGui::CollapsingHeader("Mesh")) {

				if (mesh != nullptr) {

					ImGui::Text("Mesh ID: %i", mesh->id);
					ImGui::Text("Vertices ID: %u", mesh->vertexId);
					ImGui::Text("Normals ID: %u", mesh->normalsId);
					ImGui::Text("Texture coordinates ID: %u", mesh->textureCoordId);
					ImGui::Text("Indices ID: %u", mesh->indexId);
					ImGui::NewLine();
					ImGui::Checkbox(" Show all mesh bones", &mesh->showAllBones);

					for (uint i = 0; i < mesh->boneNamesVec.size(); i++) {

						ImGui::Text("	%s", mesh->boneNamesVec[i].c_str());
						ImGui::SameLine();
						std::string auxName("##Checkbox" + std::to_string(i));
						ImGui::Checkbox(auxName.c_str(), &mesh->boneDisplayVec[i]);

					}

					ImGui::NewLine();
					ImGui::Checkbox("Activate normals display", &mesh->paintNormals);
					ImGui::NewLine();

				}

				else { ImGui::Text("This object has no mesh attached"); }

				if (ImGui::Button("Select new mesh")) {

					loadMeshMenu = true;
					selectedFilePath.clear();

				}

				if (loadMeshMenu) {

					ImGui::OpenPopup("Load mesh");

					if (ImGui::BeginPopupModal("Load mesh", &loadMeshMenu)) {

						ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
						ImGui::BeginChild("Mesh Browser", ImVec2(0, 300), true);

						std::vector<std::string> files;

						App->externalManager->DiscoverFiles(MESHES_PATH, files);
						std::sort(files.begin(), files.end());

						for (uint i = 0; i < files.size(); i++) {

							if (ImGui::TreeNodeEx(files[i].c_str(), ImGuiTreeNodeFlags_Leaf)) {

								if (ImGui::IsItemClicked()) {

									selectedFilePath = (MESHES_PATH + files[i]).c_str();

									if (ImGui::IsMouseDoubleClicked(0)) {

										App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)selectedFilePath.c_str());
										loadMeshMenu = false;

									}

								}

								ImGui::TreePop();

							}

						}

						ImGui::EndChild();
						ImGui::PopStyleVar();

						ImGui::PushItemWidth(250.f);
						ImGui::Text("%s", (char*)selectedFilePath.c_str(), FILE_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

						ImGui::PopItemWidth();
						ImGui::SameLine();

						if (ImGui::Button("Ok", ImVec2(50, 20))) {

							App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)selectedFilePath.c_str());
							loadMeshMenu = false;

						}

						ImGui::SameLine();

						if (ImGui::Button("Cancel", ImVec2(50, 20))) { selectedFilePath[0] = '\0'; }

						ImGui::EndPopup();

					}

				}

			}

			if (ImGui::CollapsingHeader("Textures")) {

				if (material != nullptr && App->texture->IsTextureRepeated(material->GetTextureName()) != 0) {

					std::string name = textureData->name + ".dds";

					ImGui::Text("Name: %s", name.c_str());
					ImGui::Text("OpenGL ID: %i", textureData->textureId);
					ImGui::Text("Width: %d", textureData->width);
					ImGui::Text("Height: %d", textureData->height);
					if (material->GetTextureName() != App->texture->checkersTexture) { if (ImGui::Button("Switch to Checkers Texture")) { material->SetTextureName(App, App->texture->checkersTexture); } }

				}

				else if (material != nullptr) {

					ImGui::NewLine();
					ImGui::Text("No texture. Add a texture:");
					ImGui::NewLine();

					if (ImGui::Button("Default texture")) { material->SetTextureName(App, App->texture->defaultTexture); }
					if (ImGui::Button("Checkers texture")) { material->SetTextureName(App, App->texture->checkersTexture); }
					if (ImGui::Button("Ahegao texture. How could this get in here")) { material->SetTextureName(App, App->texture->degenerateTexture); }

				}

				else {

					ImGui::NewLine();
					ImGui::Text("No material. Add material to have\na texture:");
					ImGui::NewLine();

					if (ImGui::Button("Add material with default texture")) {

						material = (Material*)focus->AddComponent(COMPONENT_TYPE::MATERIAL);
						material->SetTextureName(App, App->texture->defaultTexture);

					}

					if (ImGui::Button("Add material with checkers texture")) {

						material = (Material*)focus->AddComponent(COMPONENT_TYPE::MATERIAL);
						material->SetTextureName(App, App->texture->checkersTexture);

					}

					if (ImGui::Button("Add material with an ahegao texture. This is no joke. It's a meme of culture")) {

						material = (Material*)focus->AddComponent(COMPONENT_TYPE::MATERIAL);
						material->SetTextureName(App, App->texture->degenerateTexture);

					}

				}

				if (material != nullptr) {
					if (ImGui::Button("Load texture")) {

						loadTexturesMenu = true;
						selectedFilePath.clear();

					}
				}

				if (loadTexturesMenu) {

					ImGui::OpenPopup("Load texture");

					if (ImGui::BeginPopupModal("Load texture", &loadTexturesMenu)) {

						ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
						ImGui::BeginChild("Texture Browser", ImVec2(0, 300), true);

						std::vector<std::string> files;

						App->externalManager->DiscoverFiles(TEXTURES_PATH, files);
						std::sort(files.begin(), files.end());

						for (uint i = 0; i < files.size(); i++) {

							if (ImGui::TreeNodeEx(files[i].c_str(), ImGuiTreeNodeFlags_Leaf)) {

								if (ImGui::IsItemClicked()) {

									selectedFilePath = (TEXTURES_PATH + files[i]).c_str();

									if (ImGui::IsMouseDoubleClicked(0)) {

										App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)selectedFilePath.c_str());
										loadTexturesMenu = false;

									}

								}

								ImGui::TreePop();

							}

						}

						ImGui::EndChild();
						ImGui::PopStyleVar();

						ImGui::PushItemWidth(250.f);
						ImGui::Text("%s", (char*)selectedFilePath.c_str(), FILE_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

						ImGui::PopItemWidth();
						ImGui::SameLine();

						if (ImGui::Button("Ok", ImVec2(50, 20))) {

							App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)selectedFilePath.c_str());
							loadTexturesMenu = false;

						}

						ImGui::SameLine();

						if (ImGui::Button("Cancel", ImVec2(50, 20))) { selectedFilePath[0] = '\0'; }

						ImGui::EndPopup();

					}

				}

				if (mesh == nullptr) {

					ImGui::NewLine();
					ImGui::NewLine();
					ImGui::Text("Don't want to ruin the fun, but...\nThere's no mesh to apply texture to.");

				}

			}

			if (ImGui::CollapsingHeader("Animation")) {

				if (animation == nullptr) { ImGui::Text("Funny you, there's no animation."); }
				else {

					for (uint i = 0; i < animation->animationVec.size(); i++) {

						ImGui::Text("Animation %u: %s", i, animation->animationVec[i].name.c_str());
						ImGui::Text("Duration: %f", animation->animationVec[i].duration);
						ImGui::Checkbox("Animation playing", &animation->animationVec[i].playing);
						ImGui::NewLine();

					}

				}

			}

			if (ImGui::CollapsingHeader("Camera")) {

				if (camera == nullptr) {

					if (ImGui::Button("Create camera")) { /*camera = (Camera*)focus->AddComponent(COMPONENT_TYPE::CAMERA);*/ } // TODO: RIP camera

				}

				else { if (ImGui::Button("Destroy camera")) { focus->SetDeleteComponent(COMPONENT_TYPE::CAMERA); } }

			}

			if (ImGui::Checkbox("Hide / Show object", &focus->enabled)) {}

		}

		ImGui::End();
	}

}


void ManagerImGui::ReferenceWindow() {

	if (referenceMenu) {

		ImGui::Begin("References", &referenceMenu);

		ImGui::Text("Textures:");

		std::vector<TextureData> textureData = App->texture->GetTextureVector();
		for (uint i = 0; i < textureData.size(); i++) { ImGui::Text("%s = %i", textureData[i].name.c_str(), textureData[i].reference); }

		ImGui::End();

	}

}

void ManagerImGui::LoadFileMenu(const char* directory, const char* extension) {

	ImGui::OpenPopup("Load File");

	if (ImGui::BeginPopupModal("Load File", &loadFileMenu)) {

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);

		DrawDirectoryRecursively(directory, extension);

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(250.f);
		ImGui::Text("%s", (char*)selectedFilePath.c_str(), FILE_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Ok", ImVec2(50, 20))) {

			App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)selectedFilePath.c_str());
			loadFileMenu = false;

		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(50, 20))) { selectedFilePath[0] = '\0'; }

		ImGui::SameLine();
		if ((ImGui::Button("Delete", ImVec2(70, 20)) || App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) && selectedFilePath.empty() == false) {

			deletedFileName = selectedFilePath;
			App->externalManager->RemoveFileByName(selectedFilePath.c_str());
			App->eventManager->GenerateEvent(EVENT_ENUM::FILE_DELETED, EVENT_ENUM::NULL_EVENT, (char*)deletedFileName.c_str());
			selectedFilePath.clear();

		}

		ImGui::EndPopup();

	}

}


void ManagerImGui::DrawDirectoryRecursively(const char* directory, const char* extension) {

	std::vector<std::string> files;
	std::vector<std::string> dirs;
	bool openNode;

	std::string dir((directory) ? directory : "");
	dir += "/";

	App->externalManager->DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {

		openNode = ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str());

		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {

			ImGui::BeginTooltip();
			ImGui::Text("Hovering on %s.", (dir + (*it) + "/").c_str());
			ImGui::EndTooltip();

			if (ImGui::IsMouseReleased(0)) {

				std::string fileName, extension;
				App->externalManager->SplitFilePath(selectedFilePath.c_str(), nullptr, &fileName, &extension);
				dragDropFile = dir + (*it) + "/" + fileName + "." + extension;

				if (App->externalManager->IsDirectory(dragDropFile.c_str()) == false) {

					App->eventManager->GenerateEvent(EVENT_ENUM::FILE_DRAGGED_IN_LOAD_WINDOW, EVENT_ENUM::NULL_EVENT, (char*)dragDropFile.c_str());
					selectedFilePath.clear();

				}

			}

		}

		if (openNode) {

			DrawDirectoryRecursively((dir + (*it)).c_str(), extension);

			ImGui::TreePop();

		}

	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool ok = true;

		if (extension && str.substr(str.find_last_of(".") + 1) != extension) { ok = false; }

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf)) {

			if (ImGui::IsItemClicked()) {

				selectedFilePath = dir + str.c_str();

				if (ImGui::IsMouseDoubleClicked(0)) {

					App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)selectedFilePath.c_str());
					loadFileMenu = false;

				}

			}

			ImGui::TreePop();
		}
	}

}


void ManagerImGui::HierarchyManagement() {

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && App->editorScene->GetFocus() != nullptr && App->editorScene->GetFocus() != itemHovered && MouseIsInside(hierarchyWindowPos, hierarchyWindowSize) && hasHierarchyFocus) {

		if (App->editorScene->GetFocus()->parent != nullptr) {

			for (uint i = 0; i < App->editorScene->GetFocus()->parent->childs.size(); i++) {

				if (App->editorScene->GetFocus()->parent->childs[i]->id == App->editorScene->GetFocus()->id) {

					App->editorScene->GetFocus()->parent->childs.erase(App->editorScene->GetFocus()->parent->childs.begin() + i);
					i = App->editorScene->GetFocus()->parent->childs.size();

				}

			}

		}

		else {

			for (uint i = 0; i < App->editorScene->rootGameObjectsVec.size(); i++) {

				if (App->editorScene->rootGameObjectsVec[i]->id == App->editorScene->GetFocus()->id) {

					App->editorScene->rootGameObjectsVec.erase(App->editorScene->rootGameObjectsVec.begin() + i);
					i = App->editorScene->rootGameObjectsVec.size();

				}

			}

		}

		App->editorScene->GetFocus()->parent = itemHovered;
		if (itemHovered != nullptr) { itemHovered->childs.push_back(App->editorScene->GetFocus()); }
		else if (itemHovered == nullptr) { App->editorScene->rootGameObjectsVec.push_back(App->editorScene->GetFocus()); }
	}

	itemFocusedLastFrame = App->editorScene->GetFocus();

}


std::string ManagerImGui::AppName() { return appName; }


bool ManagerImGui::MouseIsInside(ImVec2 position, ImVec2 size) {

	if (App->input->GetMouseX() > position.x && App->input->GetMouseX() < position.x + size.x) {

		if (App->input->GetMouseY() > position.y && App->input->GetMouseY() < position.y + size.y) {

			return true;

		}

	}

	return false;

}


