#include "Application.h"

#include <shellapi.h>

ManagerImGui::ManagerImGui(Application* app, bool start_enabled) : Module(app, start_enabled), SDL(nullptr), MathGeoLib(nullptr), sliderDt(0.0f), appName("NotThatGameEngine"),
sliderBrightness(1.0f), sliderWidth(SCREEN_WIDTH* SCREEN_SIZE), sliderHeight(SCREEN_HEIGHT* SCREEN_SIZE), vsync(true),
fullscreen(WIN_FULLSCREEN), resizable(WIN_RESIZABLE), borderless(WIN_BORDERLESS), fullDesktop(WIN_FULLSCREEN_DESKTOP), refreshRate(0),
AVX(false), AVX2(false), AltiVec(false), MMX(false), RDTSC(false), SSE(false), SSE2(false), SSE3(false), SSE41(false), SSE42(false),
showDemoWindow(false), defaultButtonsMenu(false), aboutWindow(false), configMenu(false), appActive(false), consoleMenu(false), sceneWindow(false), hierarchyWindow(true), inspectorWindow(false)
{}


ManagerImGui::~ManagerImGui()
{}


bool ManagerImGui::Init()
{
	bool ret = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Dynamically showing the version of the libs
	GLEW = "GLEW ";
	char* glewVersion = (char*)glewGetString(GLEW_VERSION);
	strcat_s((char*)GLEW.c_str(), GLEW.size() + 1 + strlen(glewVersion), glewVersion);

	ImGui = "ImGui ";
	const char* preImGuivers = ImGui::GetVersion();
	strcat_s((char*)ImGui.c_str(), ImGui.size() + 1 + strlen(preImGuivers), preImGuivers);

	SDL = (char*)"SDL 2.0.12";
	MathGeoLib = (char*)"MathGeoLib 1.5";

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


update_status ManagerImGui::PreUpdate(float dt)
{
	ImVec4 clear_color = ImVec4(0.0f, 0.15f, 0.10f, 1.00f);

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	return update_status::UPDATE_CONTINUE;
}


update_status ManagerImGui::Update(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;
	update_status ret2 = update_status::UPDATE_CONTINUE;


	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (showDemoWindow) { ImGui::ShowDemoWindow(&showDemoWindow); }	// DEMO WINDOW
	ret = DefaultButtons();
	SetMainMenuBar();
	ret2 = DefaultWindow();
	ConsoleWindow();
	SceneWindow();
	HierarchyWindow();
	InspectorWindow();

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

	return ret;
}


bool ManagerImGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}


update_status ManagerImGui::DefaultButtons()
{
	static bool wireframe = false;

	update_status ret = update_status::UPDATE_CONTINUE;

	if (defaultButtonsMenu) {		// CLOSE APP BUTTON
		ImGui::Begin("Default buttons", &defaultButtonsMenu);
		if (ImGui::Button("Close App")) { ret = update_status::UPDATE_STOP; }

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

	return ret;
}


void ManagerImGui::SetMainMenuBar()
{

	if (ImGui::BeginMainMenuBar()) {	// TODO: order those in different tabs

		if (ImGui::BeginMenu("Dev Options")) {

			if (ImGui::MenuItem("Scene Window")) { sceneWindow = !sceneWindow; }

			if (ImGui::MenuItem("Hierarchy Window")) { hierarchyWindow = !hierarchyWindow; }

			if (ImGui::MenuItem("Inspector Window")) { inspectorWindow = !inspectorWindow; }

			if (ImGui::MenuItem("Gui Demo")) { showDemoWindow = !showDemoWindow; }

			if (ImGui::MenuItem("Documentation")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/wiki", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Download latest")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/releases", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Report a bug")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/issues", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("App configuration")) { configMenu = !configMenu; }

			if (ImGui::MenuItem("General buttons")) { defaultButtonsMenu = !defaultButtonsMenu; }

			if (ImGui::MenuItem("Console output")) { consoleMenu = !consoleMenu; }

			if (ImGui::MenuItem("About")) { aboutWindow = !aboutWindow; }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create Primitive")) {

			if (ImGui::MenuItem("Cube")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_CUBE); }

			if (ImGui::MenuItem("Sphere")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_SPHERE); }

			if (ImGui::MenuItem("Pyramid")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_PYRAMID); }

			if (ImGui::MenuItem("Cylinder")) { App->eventManager->GenerateEvent(EVENT_ENUM::CREATE_CYLINDER); }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

	}

	if (aboutWindow) { AboutMenu(&aboutWindow); }
}


void ManagerImGui::AboutMenu(bool* aboutMenu) {

	ImGui::Begin("About", aboutMenu);
	ImGui::Text("NotThatGameEngine\n\nAn attempt to create a usable game engine >:3\nBy Ferran-Roger Basart i Bosch:");
	if (ImGui::Button("GitHub")) { ShellExecuteA(0, "open", "https://github.com/ferba93/", NULL, NULL, SW_SHOWNORMAL); }
	ImGui::Text("\n\nLIBRARIES:");
	if (ImGui::Button(SDL)) { ShellExecuteA(0, "open", "https://www.libsdl.org/", NULL, NULL, SW_SHOWNORMAL); }
	if (ImGui::Button(GLEW.c_str())) { ShellExecuteA(0, "open", "http://glew.sourceforge.net/index.html", NULL, NULL, SW_SHOWNORMAL); }
	if (ImGui::Button(ImGui.c_str())) { ShellExecuteA(0, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL); }
	if (ImGui::Button(MathGeoLib)) { ShellExecuteA(0, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL); }
	ImGui::Text("\n\nLICENSE:\n\nMIT License\n\nCopyright(c) 2020 Ferran - Roger Basart i Bosch\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this softwareand associated documentation files(the 'Software'), \nto deal in the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell\ncopies of the Software, and to permit persons to whom the Software is furnished\nto do so, subject to the following conditions : \n");
	ImGui::Text("\tThe above copyright noticeand this permission notice shall be included in all\n\tcopies or substantial portions of the Software.\n");
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\nINCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
	ImGui::End();
}


update_status ManagerImGui::DefaultWindow() {

	update_status ret = update_status::UPDATE_CONTINUE;
	static char organization[25] = "UPC CITM";
	char title[25];

	if (configMenu) {
		ImGui::Begin("Configuration", &configMenu);

		if (ImGui::BeginMenu("Options")) {
			ImGui::EndMenu();
		}

		if (ImGui::CollapsingHeader("Application")) {

			if (ImGui::InputText("App name", appName, IM_ARRAYSIZE(appName))) {
				App->window->SetTitle(appName);
			}

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

		if (ImGui::CollapsingHeader("Hardware")) {
			ImGui::Text("SDL 2.0.12");
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


void ManagerImGui::ConsoleWindow() {

	if (consoleMenu) {
		ImGui::Begin("Console", &consoleMenu);

		for (int i = 0; i < App->consoleVecSize; i++) {
			ImGui::Text(App->consoleVec[0].c_str());
		}
		ImGui::End();
	}
}


void ManagerImGui::SceneWindow() {

	ImVec2 vec2(App->window->width * 0.75, App->window->height * 0.75);

	if (sceneWindow) {

		ImGui::Begin("Scene", &sceneWindow);

		ImGui::Image((ImTextureID*)App->renderer3D->sceneTextureId, vec2);

		ImGui::End();
	}
}


void ManagerImGui::HierarchyWindow() {

	if (hierarchyWindow) {

		ImGui::Begin("Hierarchy", &hierarchyWindow);



		ImGui::End();
	}

}


void ManagerImGui::InspectorWindow() {

	if (inspectorWindow) {

		ImGui::Begin("Inspector", &inspectorWindow);

		ImGui::End();
	}

}


std::string ManagerImGui::AppName() { return appName; }


