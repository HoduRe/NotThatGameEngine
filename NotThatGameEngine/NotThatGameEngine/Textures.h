#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "glew-2.1.0/include/GL/glew.h"
#include "Module.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/postprocess.h"

struct TextureData {

	TextureData(GLuint& _id, const char* textureName , GLenum _textureType);

	std::string name;
	GLenum textureType;
	GLuint textureId;
};

class Texture : public Module
{

public:

	Texture(Application* app, bool start_enabled = true);
	~Texture();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	uint LoadTexture(const char* path, const char* textureName = nullptr, GLenum _textureType = GL_DIFFUSE);

public:

	std::vector<TextureData> textureVec;

private:

	aiLogStream stream;

};


#endif