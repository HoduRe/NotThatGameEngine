#ifndef __MODELMANAGER_H__
#define __MODELMANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "Primitives.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/postprocess.h"

/*
#ifndef TEXTURE_H
#define	TEXTURE_H

#include <string>

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	Magick::Image m_image;
	Magick::Blob m_blob;
};


#endif	// TEXTURE_H

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
	m_textureTarget = TextureTarget;
	m_fileName      = FileName;
}


bool Texture::Load()
{
	try {
		m_image.read(m_fileName);
		m_image.write(&m_blob, "RGBA");
	}
	catch (Magick::Error& Error) {
		std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
		return false;
	}

	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	glTexImage2D(m_textureTarget, 0, GL_RGBA, m_image.columns(), m_image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(m_textureTarget, 0);

	return true;
}

void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}
*/

struct Mesh {

	struct SubMeshes {
		GLuint vertexBufferId;
		GLuint indexBufferId;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	uint vramIndexId = 0;
	uint numIndex = 0;
	uint* index = nullptr;

	uint vramUniqueVertices = 0;
	uint numVertices = 0;
	float* vertices = nullptr;

	std::vector<SubMeshes> subMeshes;
	std::vector textures;

};

class ModelManager : public Module
{

public:

	ModelManager(Application* app, bool start_enabled = true);
	~ModelManager();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadModel(std::string path);

public:

private:

	void ExecuteEvent(EVENT_ENUM eventId);

private:
	aiLogStream stream;
	Mesh testMesh;
};

#endif