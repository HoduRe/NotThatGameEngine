#ifndef __Audio_H__
#define __Audio_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

class Audio : public Module
{
public:

	Audio(Application* app, bool start_enabled = true);
	~Audio();

	bool Init();
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

private:


public:
	int sfxAdjustment;
	int musicAdjustment;
};

#endif // __Audio_H__