// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

// ---------------------------------------------
float Timer::Read()
{
	float SDLTicks = SDL_GetTicks();
	
	if(running == true)
	{
		return SDLTicks - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}


