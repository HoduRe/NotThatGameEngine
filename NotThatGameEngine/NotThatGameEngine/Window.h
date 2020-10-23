#ifndef __Window_H__
#define __Window_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

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
	void ExecuteEvent(EVENT_ENUM eventId);

private:

};

#endif // __Window_H__