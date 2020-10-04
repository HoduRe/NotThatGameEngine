#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include <shellapi.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleImGui::ModuleImGui(Application* app, bool start_enabled) : Module(app, start_enabled)
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

	SDL = (char*)"SDL ";
	GLEW = (char*)"GLEW ";
	ImGui = (char*)"ImGui ";
	MathGeoLib = (char*)"MathGeoLib ";

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


	return update_status::UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModuleImGui::Update(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;
	static bool showDemoWindow = true;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

//	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 220, 0));		// Cuando cierro la ventana de demo, la ventana de close app se teleporta a su posición inicial: sustitye esto por código que solo lo setea al compilar
//	ImGui::SetNextWindowSize(ImVec2(180, 70));					// Lo más probable es que haya un buffer de siguiente nuevo me lo pones a esta posición, y al quitar el anterior, se mueve el de close app en la lista, y se lee como "nuevo"

	ret = DefaultMenus(&showDemoWindow);
	SetMainMenuBar(&showDemoWindow);

	ImGui::EndFrame();

	return ret;
}

// ---------------------------------------------------------
update_status ModuleImGui::PostUpdate(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Rendering
	ImGui::Render();
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
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


update_status ModuleImGui::DefaultMenus(bool* demoMenu)
{
	update_status ret = update_status::UPDATE_CONTINUE;
	static bool close_app_window = true;

	if (*demoMenu) {		// DEMO WINDOW
		ImGui::ShowDemoWindow(demoMenu);
	}

	if (close_app_window) {		// CLOSE APP BUTTON
		ImGui::Begin("Close app", &close_app_window);
		if (ImGui::Button("Close Button"))
			ret = update_status::UPDATE_STOP;
		ImGui::End();
	}

	return ret;
}


void ModuleImGui::SetMainMenuBar(bool* demoMenu)
{
	static bool aboutWindow = false;

	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("Dev Help")) {

			if (ImGui::MenuItem("Gui Demo")) { *demoMenu = ! *demoMenu; }

			if (ImGui::MenuItem("Documentation")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/wiki", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Download latest")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/releases", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("Report a bug")) { ShellExecuteA(0, "open", "https://github.com/ferba93/NotThatGameEngine/issues", NULL, NULL, SW_SHOWNORMAL); }

			if (ImGui::MenuItem("About")) { if (!aboutWindow) { aboutWindow = !aboutWindow; } }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (aboutWindow) { AboutMenu(&aboutWindow); }
}


void ModuleImGui::AboutMenu(bool* aboutMenu) {

	ImGui::Begin("About", aboutMenu);
	ImGui::Text("NotThatGameEngine\n\nAn attempt to create a usable game engine >:3\nBy Ferran-Roger Basart i Bosch:");
	if(ImGui::Button("GitHub")){ ShellExecuteA(0, "open", "https://github.com/ferba93/", NULL, NULL, SW_SHOWNORMAL); }
	ImGui::Text("\n\nLIBRARIES:");
	if(ImGui::Button (SDL)){ ShellExecuteA(0, "open", "https://www.libsdl.org/", NULL, NULL, SW_SHOWNORMAL); }
	if (ImGui::Button(GLEW)) { ShellExecuteA(0, "open", "http://glew.sourceforge.net/index.html", NULL, NULL, SW_SHOWNORMAL); }
	if (ImGui::Button(ImGui)) { ShellExecuteA(0, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL); }
	if (ImGui::Button(MathGeoLib)) { ShellExecuteA(0, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL); }
	ImGui::Text("\n\nLICENSE:\n\nMIT License\n\nCopyright(c) 2020 Ferran - Roger Basart i Bosch\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this softwareand associated documentation files(the 'Software'), \nto deal in the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell\ncopies of the Software, and to permit persons to whom the Software is furnished\nto do so, subject to the following conditions : \n");
	ImGui::Text("\tThe above copyright noticeand this permission notice shall be included in all\n\tcopies or substantial portions of the Software.\n");
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\nINCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
	ImGui::End();
}


