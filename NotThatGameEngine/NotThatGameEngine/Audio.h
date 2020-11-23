#ifndef __Audio_H__
#define __Audio_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

class Audio : public Module {

public:

	Audio(Application* app, bool start_enabled = true);
	~Audio();

	bool Init();
	bool CleanUp();

	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	unsigned int LoadFx(const char* path);

	bool PlayFx(unsigned int fx, int repeat = 0);

private:


public:

	int sfxAdjustment;
	int musicAdjustment;

};

#endif // __Audio_H__