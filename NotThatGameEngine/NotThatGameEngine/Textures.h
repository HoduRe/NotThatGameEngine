#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"

struct TextureData {

	TextureData();
	TextureData(GLuint& _id, const char* _name, GLenum _textureType, int width, int height, int _size, int _UUID);

	GLenum textureType;
	GLuint textureId;
	std::string name;
	int width;
	int height;
	int size;

	int textureUUID;
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
	uint IsTextureRepeated(GLuint id);
	uint IsTextureRepeated(int id);
	uint IsTextureRepeated(const char* _name);
	TextureData* GetTextureData(GLuint id);

public:

	uint defaultTextureId;
	uint checkersTextureId;
	uint degenerateTextureId;

private:

	std::vector<TextureData> textureVec;

};


#endif