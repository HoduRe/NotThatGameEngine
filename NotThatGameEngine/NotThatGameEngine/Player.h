#ifndef __Player_h__
#define __Player_h__
#include "Module.h"


class Player : public Module
{
public:
	Player(Application* app, bool start_enabled = true);
	virtual ~Player();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

};

#endif