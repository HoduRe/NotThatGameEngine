#include "Application.h"

Window::Window(Application* app, bool start_enabled) : Module(app, start_enabled), window(NULL), screen_surface(NULL), width(SCREEN_WIDTH* SCREEN_SIZE), height(SCREEN_HEIGHT* SCREEN_SIZE)
{}

// Destructor
Window::~Window()
{
}

// Called before render is available
bool Window::Init()
{
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->imGui->AppName().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	App->eventManager->EventRegister(EVENT_ENUM::SCREEN_BRIGHTNESS, this);
	App->eventManager->EventRegister(EVENT_ENUM::CHANGE_WINDOW_WIDTH, this);
	App->eventManager->EventRegister(EVENT_ENUM::CHANGE_WINDOW_HEIGHT, this);
	App->eventManager->EventRegister(EVENT_ENUM::WINDOW_RESIZE, this);
	App->eventManager->EventRegister(EVENT_ENUM::FULLSCREEN, this);
	App->eventManager->EventRegister(EVENT_ENUM::RESIZABLE_WINDOW, this);
	App->eventManager->EventRegister(EVENT_ENUM::BORDERLESS_WINDOW, this);
	App->eventManager->EventRegister(EVENT_ENUM::FULLDESKTOP_WINDOW, this);

	return ret;
}

// Called before quitting
bool Window::CleanUp()
{
	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}


update_status Window::PostUpdate(float dt) {

	CheckListener(this);

	return update_status::UPDATE_CONTINUE;
}


void Window::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}



void Window::ExecuteEvent(EVENT_ENUM eventId) {

	switch (eventId) {

	case EVENT_ENUM::SCREEN_BRIGHTNESS:

		SDL_SetWindowBrightness(window, App->imGui->sliderBrightness);

		break;

	case EVENT_ENUM::CHANGE_WINDOW_WIDTH:

		width = App->imGui->sliderWidth * SCREEN_SIZE;
		SDL_SetWindowSize(window, width, height);

		break;

	case EVENT_ENUM::CHANGE_WINDOW_HEIGHT:

		height = App->imGui->sliderHeight * SCREEN_SIZE;
		SDL_SetWindowSize(window, width, height);

		break;

	case EVENT_ENUM::WINDOW_RESIZE:

		SDL_GetWindowSize(window, &width, &height);

		break;

	case EVENT_ENUM::FULLSCREEN:

		if (App->imGui->fullscreen) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			App->imGui->resizable = false;
			App->imGui->fullDesktop = false;
			App->imGui->borderless = false;
		}

		break;

	case EVENT_ENUM::RESIZABLE_WINDOW:

		if (App->imGui->resizable) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_RESIZABLE);
			App->imGui->fullscreen = false;
			App->imGui->fullDesktop = false;
			App->imGui->borderless = false;
		}

	case EVENT_ENUM::BORDERLESS_WINDOW:

		if (App->imGui->borderless) {
			SDL_SetWindowBordered(window, SDL_FALSE);
			App->imGui->fullscreen = false;
			App->imGui->fullDesktop = false;
			App->imGui->resizable = false;
		}
		else {
			SDL_SetWindowBordered(window, SDL_TRUE);
		}

	case EVENT_ENUM::FULLDESKTOP_WINDOW:

		if (App->imGui->fullDesktop) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			App->imGui->fullscreen = false;
			App->imGui->resizable = false;
			App->imGui->borderless = false;
		}

		break;

	default:
		break;
	}

	App->renderer3D->OnResize(width, height);
}


