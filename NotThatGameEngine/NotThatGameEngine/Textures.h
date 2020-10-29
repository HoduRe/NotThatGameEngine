#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"

struct TextureData {

	TextureData();
	TextureData(GLuint& _id, const char* textureName , GLenum _textureType, int width, int height);

	std::string path;
	GLenum textureType;
	GLuint textureId;
	int width;
	int height;
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
	TextureData GetTextureData(GLuint id);

public:

	uint defaultTextureId;
	uint checkersTextureId;
	uint degenerateTextureId;

private:

	std::vector<TextureData> textureVec;

};


#endif