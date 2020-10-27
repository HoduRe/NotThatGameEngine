#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"

struct TextureData {

	TextureData(GLuint& _id, const char* textureName , GLenum _textureType);

	std::string path;
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

	void AddTexture(TextureData* texture);
	uint IsTextureRepeated(const char* path);

public:

	uint defaultTextureId;

private:

	std::vector<TextureData> textureVec;

};


#endif