#ifndef __OPENGL_FUNCTIONALTY_H__
#define __OPENGL_FUNCTIONALTY_H__

#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"

#include "MathGeoLib/include/MathGeoLib.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#include <gl/GL.h>
#include <gl/GLU.h>

class Mesh;

bool SetOpenGL();

void LoadDataBufferFloat(int bufferType, GLuint* id, int size, float* data);
void LoadDataBufferUint(int bufferType, GLuint* id, int size, GLuint* data);
void LoadGLTexture(GLuint* _id, int width, int height, int bpp, int format, ILubyte* data);

void DrawMeshes(Mesh& mesh, float4x4 worldTransform, GLuint textureId);

#endif