#ifndef __TEXTUREDATA_H__
#define __TEXTUREDATA_H__

#include "glew-2.1.0/include/GL/glew.h"
#include "Resource.h"

class TextureData : public Resource {

public:

	TextureData(long long int id);
	TextureData(GLuint& _id, long long int resourceId, const char* _name, GLenum _textureType, int width, int height, int _size);
	~TextureData();

public:

	GLenum textureType;
	GLuint textureId;
	std::string name;
	int width;
	int height;
	int size;

};

#endif