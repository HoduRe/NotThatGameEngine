#include "Application.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment ( lib, "Devil/libx86/DevIL.lib" )
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )


Texture::Texture(Application* app, bool start_enabled) : Module(app, start_enabled), textureVec() {}


Texture::~Texture() { textureVec.clear(); }


bool Texture::Init() {			// OpenGL has not been initialized yet

	bool ret = true;

	ilInit();
	iluInit();
	ilutInit();

	return ret;
}


bool Texture::Start() {

	LoadTexture("Library/Textures/Baker_house.png");

	return true;
}


bool Texture::CleanUp() { return true; }


update_status Texture::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


uint Texture::LoadTexture(const char* path, const char* textureName, GLenum _textureType) {

	uint imageTest;
	ilGenImages(1, &imageTest);
	ilBindImage(imageTest);
	if (ilLoadImage(path) == IL_TRUE) {}
	else { LOG("Image with id: %u failed to load.\n", imageTest); }

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &imageTest);
	glBindTexture(GL_TEXTURE_2D, imageTest);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &imageTest);

	if (textureName == nullptr) { textureName = path; }
	textureVec.push_back(TextureData (imageTest, textureName, _textureType));

	return imageTest;
}


TextureData::TextureData(GLuint& _id, const char* textureName, GLenum _textureType) {
	textureId = _id;
	name = textureName;
	textureType = _textureType;
}



