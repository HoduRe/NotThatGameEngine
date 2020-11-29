#include "TextureData.h"

TextureData::TextureData(long long int id) : Resource(id, ResourceEnum::TEXTURE), textureId(0), textureType(0), width(0), height(0), size(0), name(), reference(0) {}


TextureData::TextureData(GLuint& _id, long long int resourceId, const char* _name, GLenum _textureType, int _width, int _height, int _size) : Resource(resourceId, ResourceEnum::TEXTURE),
	textureId(_id), name(_name), textureType(_textureType), width(_width), height(_height), size(_size), reference(0) {}


TextureData::~TextureData(){}

