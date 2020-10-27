#include "Application.h"
#include "ObjectLoader.h"


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
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	defaultTextureId = LoadTexture(App, "Library/Textures/Alex.png");
	App->eventManager->GenerateEvent(EVENT_ENUM::DEFAULT_TEXTURE_LOADED);

	return true;
}


bool Texture::CleanUp() { return true; }


update_status Texture::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


void Texture::AddTexture(TextureData* texture) { textureVec.push_back(*texture); }


uint Texture::IsTextureRepeated(const char* path) {
	
	for (int i = textureVec.size() - 1; i > -1; i--) {
	
		if (textureVec[i].path == path) { return textureVec[i].textureId; }

	}

	return 0;
}


TextureData::TextureData(GLuint& _id, const char* textureName, GLenum _textureType) : textureId(_id), path(textureName), textureType(_textureType) { }



