#ifndef __OBJECTLOADER_H__
#define __OBJECTLOADER_H__

#include "Globals.h"

class GameObject;
class Application;

	GameObject* LoadModel(Application* app, const char* path, const char* buffer = nullptr, uint size = 0, GameObject* parent = nullptr, bool enabled = true);
	void LoadTexture(Application* app, const char* path);

	bool LoadScene(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path);

#endif
