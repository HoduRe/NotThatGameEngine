#include "OpenGLFuncionality.h"
#include "Mesh.h"

#pragma comment( lib, "glew-2.1.0/lib/Release/Win32/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

bool InitOpenGL() {

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) { return false; }

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
}


void LoadDataBufferFloat(int bufferType, GLuint* id, int size, float* data) {

	glGenBuffers(1, id);
	glBindBuffer(bufferType, *id);
	glBufferData(bufferType, size, data, GL_STATIC_DRAW);
	glBindBuffer(bufferType, 0);

}


void LoadDataBufferUint(int bufferType, GLuint* id, int size, uint* data) {

	glGenBuffers(1, id);
	glBindBuffer(bufferType, *id);
	glBufferData(bufferType, size, data, GL_STATIC_DRAW);
	glBindBuffer(bufferType, 0);

}


void DrawMeshes(Mesh& mesh) {

	glEnableClientState(GL_VERTEX_ARRAY);	// TODO: Do this sepparetely in EditorScene, before and after calling the GameObjects PostUpdates, in case it's too CPU consuming
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexId);
	glBindBuffer(GL_NORMAL_ARRAY, mesh.normalsId);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, mesh.textureCoordId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexId);
	//glBindTexture(GL_TEXTURE_2D, mesh.textureId);

	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}


