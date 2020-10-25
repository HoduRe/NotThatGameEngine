#ifndef __OPENGL_FUNCTIONALTY_H__
#define __OPENGL_FUNCTIONALTY_H__

#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

struct SubMeshes;

bool InitOpenGL();

void LoadVertexBuffer(GLuint* id, int size, float* data);
void LoadNormalBuffer(GLuint* id, int size, float* data);
void LoadTextureCoordBuffer(GLuint* id, int size, float* data);
void LoadIndexBuffer(GLuint* id, int size, uint* data);

void DrawMeshes(SubMeshes& subMesh);

#endif