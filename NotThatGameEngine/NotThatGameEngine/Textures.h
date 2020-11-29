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
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void AddTexture(TextureData* texture);
	uint IsTextureRepeated(GLuint id);
	uint IsTextureRepeated(std::string _name);
	TextureData* GetTextureData(GLuint id);
	TextureData* GetTextureData(std::string _name);
	std::vector<TextureData> GetTextureVector();
	void IncreaseTextureCount(std::string textureName);
	void DecreaseTextureCount(std::string textureName);

public:

	std::string defaultTexture;
	std::string checkersTexture;
	std::string degenerateTexture;

private:

	std::vector<TextureData> textureVec;

};


#endif