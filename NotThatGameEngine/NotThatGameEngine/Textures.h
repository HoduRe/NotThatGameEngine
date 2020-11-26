#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "TextureData.h"

class Texture : public Module {

public:

	Texture(Application* app, bool start_enabled = true);
	~Texture();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void AddTexture(TextureData* texture);
	uint IsTextureRepeated(GLuint id);
	uint IsTextureRepeated(const char* _name);
	TextureData* GetTextureData(GLuint id);
	std::vector<TextureData> GetTextureVector();

public:

	uint defaultTextureId;
	uint checkersTextureId;
	uint degenerateTextureId;

private:

	std::vector<TextureData> textureVec;

};


#endif