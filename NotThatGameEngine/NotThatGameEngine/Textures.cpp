#include "Application.h"
#include "Load.h"


Texture::Texture(Application* app, bool start_enabled) : Module(app, start_enabled), textureVec(), defaultTextureId(0), checkersTextureId(0), degenerateTextureId(0) {}


Texture::~Texture() { textureVec.clear(); }


TextureData::TextureData() : textureId(0), textureType(0), width(0), height(0), size(0), name() {}


TextureData::TextureData(GLuint& _id, const char* _name, GLenum _textureType, int _width, int _height, int _size) :
	textureId(_id), name(_name), textureType(_textureType), width(_width), height(_height), size(_size) {}


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


bool Texture::Start() {

	for (int i = 0; i < textureVec.size(); i++) {

		if (textureVec[i].name == "Alex") { defaultTextureId = textureVec[i].textureId; }
		else if (textureVec[i].name == "Checker") { checkersTextureId = textureVec[i].textureId; }
		else if (textureVec[i].name == "Degenerate") { degenerateTextureId = textureVec[i].textureId; }

	}

	App->eventManager->GenerateEvent(EVENT_ENUM::DEFAULT_TEXTURE_LOADED);

	return true;
}


bool Texture::CleanUp() {

	glDeleteTextures(textureVec.size(), &textureVec[0].textureId);
	return true;

}


update_status Texture::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


void Texture::AddTexture(TextureData* texture) { textureVec.push_back(*texture); }


uint Texture::IsTextureRepeated(GLuint id) {

	for (int i = textureVec.size() - 1; i > -1; i--) {

		if (textureVec[i].textureId == id) { return textureVec[i].textureId; }

	}

	return 0;
}


uint Texture::IsTextureRepeated(const char* _name) {

	for (int i = textureVec.size() - 1; i > -1; i--) {

		if (textureVec[i].name == _name) {

			LOG("Texture with name %s repeated.\n", _name);
			return textureVec[i].textureId;

		}

	}

	return 0;
}


TextureData* Texture::GetTextureData(GLuint id) {

	int size = textureVec.size();
	for (int i = 0; i < size; i++) { if (textureVec[i].textureId == id) { return &textureVec[i]; } }

}


std::vector<TextureData> Texture::GetTextureVector() { return textureVec; }

