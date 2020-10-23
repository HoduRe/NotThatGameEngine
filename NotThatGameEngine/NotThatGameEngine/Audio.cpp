#include "SDL/include/SDL.h"
#include "Audio.h"

Audio::Audio(Application* app, bool start_enabled) : Module(app, start_enabled), sfxAdjustment(0), musicAdjustment(0)
{}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Init()
{
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		ret = false;
	}

	return ret;
}

// Called before quitting
bool Audio::CleanUp()
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	return ret;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	return ret;
}

// Play WAV
bool Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	return ret;
}