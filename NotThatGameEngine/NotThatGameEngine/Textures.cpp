#include "Application.h"


Texture::Texture(Application* app, bool start_enabled) : Module(app, start_enabled), textureVec(), defaultTextureId(0) {}


Texture::~Texture() { textureVec.clear(); }


bool Texture::Init() {	// OpenGL has not been initialized yet

	bool ret = true;

	return ret;
}


bool Texture::Start() {

	ilInit();
	iluInit();
	ilutInit();

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	defaultTextureId = LoadTexture("Library/Textures/Alex.png");
	App->eventManager->GenerateEvent(EVENT_ENUM::DEFAULT_TEXTURE_LOADED);

	return true;
}


bool Texture::CleanUp() { return true; }


update_status Texture::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


uint Texture::LoadTexture(const char* path, GLenum _textureType) {

	uint imageTest;

	for (int i = textureVec.size() - 1; i > -1; i--) { if (textureVec[i].path == path) { return textureVec[i].textureId; } }

	ilGenImages(1, &imageTest);
	ilBindImage(imageTest);

	if (ilLoadImage(path) == IL_TRUE) {}
	else {
		LOG("Image with id: %u failed to load.\n", imageTest);
		return defaultTextureId;
	}

	LoadGLTexture(&imageTest, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());

	ilDeleteImages(1, &imageTest);

	textureVec.push_back(TextureData(imageTest, path, _textureType));

	return imageTest;
}


TextureData::TextureData(GLuint& _id, const char* textureName, GLenum _textureType) : textureId(_id), path(textureName), textureType(_textureType) { }



