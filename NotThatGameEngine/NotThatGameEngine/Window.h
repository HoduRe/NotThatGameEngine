#ifndef __Window_H__
#define __Window_H__

#include "Module.h"

class Application;
struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~Window();

	bool Init();
	update_status PostUpdate(float dt);
	bool CleanUp();

	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	int width;
	int height;

private:
	bool ExecuteEvent(EVENT_ENUM eventId, void* var);

private:

};

#endif // __Window_H__