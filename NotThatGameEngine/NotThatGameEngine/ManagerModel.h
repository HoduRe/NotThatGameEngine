#ifndef __ManagerModel_H__
#define __ManagerModel_H__

#include "Module.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/postprocess.h"

#include "Mesh.h"

class Texture;

class ManagerModel : public Module
{

public:

	ManagerModel(Application* app, bool start_enabled = true);
	~ManagerModel();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadModel(const char* path);
	uint LoadTexture(const char* path, Texture* textureContainer = nullptr, GLenum type = GL_DIFFUSE);

public:

private:

	bool ExecuteEvent(EVENT_ENUM eventId, void* var);

private:
	aiLogStream stream;
	Mesh testMesh;
};

#endif