#include "OpenGLFuncionality.h"
#include "Mesh.h"

#pragma comment ( lib, "Devil/libx86/DevIL.lib" )
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )

#pragma comment( lib, "glew-2.1.0/lib/Release/Win32/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

bool SetOpenGL() {

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) { return false; }

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
}


void LoadDataBufferFloat(int bufferType, GLuint* id, int size, float* data) {

	glGenBuffers(1, id);
	glBindBuffer(bufferType, *id);
	glBufferData(bufferType, sizeof(float) * size, data, GL_STATIC_DRAW);
	glBindBuffer(bufferType, 0);

}


void LoadDataBufferUint(int bufferType, GLuint* id, int size, GLuint* data) {

	glGenBuffers(1, id);
	glBindBuffer(bufferType, *id);
	glBufferData(bufferType, sizeof(GLuint) * size, data, GL_STATIC_DRAW);
	glBindBuffer(bufferType, 0);

}


void LoadGLTexture(GLuint* _id, int width, int height, int bpp, int format, ILubyte* data) {

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, _id);
	glBindTexture(GL_TEXTURE_2D, *_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, bpp, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

}


void DrawMeshes(Mesh& mesh, float4x4 worldTransform, GLuint textureId) {

	glPushMatrix();
	glMultMatrixf((float*)&worldTransform.Transposed());

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (mesh.normalsId != 0) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.normalsId);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (mesh.textureCoordId != 0) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.textureCoordId);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	if (textureId != 0) { glBindTexture(GL_TEXTURE_2D, textureId); }

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexId);

	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, NULL);

	if (textureId != 0) { glBindTexture(GL_TEXTURE_2D, 0); }
	if (mesh.normalsId != 0) { glDisableClientState(GL_NORMAL_ARRAY); }
	if (mesh.textureCoordId != 0) { glDisableClientState(GL_TEXTURE_COORD_ARRAY); }

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();

}


void DrawLines(float4x4 worldTransform, GLuint debugId, std::vector<float> debugVec) {

	if (debugId != 0) {

		glPushMatrix();
		glMultMatrixf((float*)&worldTransform.Transposed());

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, debugId);

		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glColor3f(80.0f, 0.0f, 0.0f);
		glDrawArrays(GL_LINES, NULL, debugVec.size());

		glColor3f(1.0f, 1.0f, 1.0f);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix();

	}

}