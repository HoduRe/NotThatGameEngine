#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleImGui::ModuleImGui(Application* app, bool start_enabled) : Module(app, start_enabled), show_demo_window(true)
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
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

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
	return update_status::UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModuleImGui::PostUpdate(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	static bool close_app_window = true;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 220, 0));
	ImGui::SetNextWindowSize(ImVec2(180, 70));

	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	if (close_app_window) {
		ImGui::Begin("Close app", &close_app_window);
		if (ImGui::Button("CLOSE BUTTON"))
			ret = update_status::UPDATE_STOP;
		ImGui::End();
	}


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

