#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include <shellapi.h>

ModuleImGui::ModuleImGui(Application* app, bool start_enabled) : Module(app, start_enabled), SDL(nullptr), MathGeoLib(nullptr), sliderDt(0.0f), appName("NotThatGameEngine"),
	sliderBrightness(1.0f), sliderWidth(SCREEN_WIDTH * SCREEN_SIZE), sliderHeight(SCREEN_HEIGHT * SCREEN_SIZE), vsync(true),
	fullscreen(WIN_FULLSCREEN), resizable(WIN_RESIZABLE), borderless(WIN_BORDERLESS), fullDesktop(WIN_FULLSCREEN_DESKTOP), refreshRate(0),
	AVX(false), AVX2(false), AltiVec(false), MMX(false), RDTSC(false), SSE(false), SSE2(false), SSE3(false), SSE41(false), SSE42(false),
	showDemoWindow(false), defaultButtonsMenu (false), aboutWindow (false), configMenu(false), appActive(false), consoleMenu (false)
{}

// Destructor
ModuleImGui::~ModuleImGui()
{}

// Render not available yet----------------------------------
bool ModuleImGui::Init()
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
	SSE41= SDL_HasSSE41();
	SSE42 = SDL_HasSSE42();

	return ret;
}

// ---------------------------------------------------------
bool ModuleImGui::Start()
{
	return true;
}

// ---------------------------------------------------------
update_status ModuleImGui::PreUpdate(float dt)
{
	ImVec4 clear_color = ImVec4(0.0f, 0.15f, 0.10f, 1.00f);

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	return update_status::UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModuleImGui::Update(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;
	update_status ret2 = update_status::UPDATE_CONTINUE;
	

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	//	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 220, 0));		// Cuando cierro la ventana de demo, la ventana de close app se teleporta a su posición inicial: sustitye esto por código que solo lo setea al compilar
	//	ImGui::SetNextWindowSize(ImVec2(180, 70));					// Lo más probable es que haya un buffer de siguiente nuevo me lo pones a esta posición, y al quitar el anterior, se mueve el de close app en la lista, y se lee como "nuevo"

	if (showDemoWindow) { ImGui::ShowDemoWindow(&showDemoWindow); }	// DEMO WINDOW
	ret = DefaultButtons();
	SetMainMenuBar();
	ret2 = DefaultWindow();
	ConsoleWindow();

	ImGui::EndFrame();

	if (ret == update_status::UPDATE_CONTINUE && ret2 == update_status::UPDATE_CONTINUE) { return ret; }
	else{ return update_status::UPDATE_STOP; }
}

// ---------------------------------------------------------
update_status ModuleImGui::PostUpdate(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->window);	// Swaps current window with the other OpenGL (by default it uses double-buffered contexts)

	return ret;
}

// Called before quitting
bool ModuleImGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}


update_status ModuleImGui::DefaultButtons()
{
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

		ImGui::End();
	}

	return ret;
}


void ModuleImGui::SetMainMenuBar()
{

	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("Dev Options")) {

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

		ImGui::EndMainMenuBar();
	}

	if (aboutWindow) { AboutMenu(&aboutWindow); }
}


void ModuleImGui::AboutMenu(bool* aboutMenu) {

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


update_status ModuleImGui::DefaultWindow() {

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
			if (ImGui::Checkbox("Active", &appActive)){ if (appActive == false) { ret = update_status::UPDATE_STOP; } }
			if (ImGui::SliderFloat("Brightness", &sliderBrightness, 0.0f, 1.0f)) { App->eventManager->GenerateEvent(EVENT_ENUM::SCREEN_BRIGHTNESS); }
			if (ImGui::SliderInt("Width", &sliderWidth, 0, 1980)) { App->eventManager->GenerateEvent(EVENT_ENUM::CHANGE_WINDOW_WIDTH); }
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


void ModuleImGui::ConsoleWindow() {

	if (consoleMenu) {
		ImGui::Begin("Console", &consoleMenu);

		for (int i = 0; i < App->consoleVecSize; i++) {
			ImGui::Text(App->consoleVec[0].c_str());
		}
		ImGui::End();
	}
}


std::string ModuleImGui::AppName() { return appName; }


