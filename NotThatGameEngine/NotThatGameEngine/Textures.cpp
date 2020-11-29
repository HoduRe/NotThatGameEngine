#include "Application.h"
#include "Textures.h"
#include "Load.h"
#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"
#include "ManagerEvent.h"

Texture::Texture(Application* app, bool start_enabled) : Module(app, start_enabled), textureVec(), defaultTexture("Alex"), checkersTexture("Checker"), degenerateTexture("Degenerate") {}


Texture::~Texture() { textureVec.clear(); }


bool Texture::Init() {	// OpenGL has not been initialized yet

	bool ret = true;

	ilInit();
	iluInit();
	ilutInit();

	LOG("DevIL libraries initialized.\n");

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	return ret;
}


bool Texture::Start() { return true; }


bool Texture::CleanUp() {

	if (textureVec.size() != 0) { glDeleteTextures(textureVec.size(), &textureVec[0].textureId); }
	return true;

}


update_status Texture::PreUpdate(float dt) {

	for (int i = 0; i < textureVec.size(); i++) {

		if (textureVec[i].reference <= 0 && textureVec[i].name != defaultTexture && textureVec[i].name != checkersTexture && textureVec[i].name != degenerateTexture) {

			glDeleteTextures(1, (const GLuint*)&textureVec[i].id);
			textureVec.erase(textureVec.begin() + i);
			i--;

		}

	}

	return update_status::UPDATE_CONTINUE;

}


update_status Texture::Update(float dt) { return update_status::UPDATE_CONTINUE; }


void Texture::AddTexture(TextureData* texture) { textureVec.push_back(*texture); }


uint Texture::IsTextureRepeated(GLuint id) {

	for (int i = textureVec.size() - 1; i > -1; i--) {

		if (textureVec[i].textureId == id) { return textureVec[i].textureId; }

	}

	return 0;
}


uint Texture::IsTextureRepeated(std::string _name) {

	for (int i = textureVec.size() - 1; i > -1; i--) {

		if (textureVec[i].name == _name) { return textureVec[i].textureId; }

	}

	return 0;
}


TextureData* Texture::GetTextureData(GLuint id) {

	int size = textureVec.size();
	for (int i = 0; i < size; i++) { if (textureVec[i].textureId == id) { return &textureVec[i]; } }

	return nullptr;

}


TextureData* Texture::GetTextureData(std::string name) {

	int size = textureVec.size();
	for (int i = 0; i < size; i++) { if (textureVec[i].name == name) { return &textureVec[i]; } }

	return nullptr;

}


std::vector<TextureData> Texture::GetTextureVector() { return textureVec; }


void Texture::IncreaseTextureCount(std::string textureName) { GetTextureData(textureName)->reference++; }


void Texture::DecreaseTextureCount(std::string textureName) { GetTextureData(textureName)->reference--; }

