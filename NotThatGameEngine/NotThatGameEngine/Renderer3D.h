#ifndef __MODULERENDER3D__
#define __MODULERENDER3D__

#include "Module.h"
#include "Light.h"
#include "OpenGLFuncionality.h"

#define MAX_LIGHTS 8

class Mesh;

class Renderer3D : public Module
{
public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void SetFrameBuffer(GLuint buffer);

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	GLuint frameBufferId;
	GLuint sceneTextureId;
	GLuint depthStencilId;

private:
	void DrawGrid();
};

#endif