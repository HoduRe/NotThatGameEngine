#ifndef __MODULERENDER3D__
#define __MODULERENDER3D__

#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "Primitives.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment( lib, "glew-2.1.0/lib/Release/Win32/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#define MAX_LIGHTS 8

struct Mesh;

class Renderer3D : public Module
{
public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	void DrawMesh(Mesh mesh);

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