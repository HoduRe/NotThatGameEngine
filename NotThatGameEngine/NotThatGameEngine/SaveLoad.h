#ifndef __SAVELOAD_H__
#define __SAVELOAD_H__

#include "Globals.h"

class Component;
class Mesh;
class Material;
class TextureData;

namespace DataSaving {

	void SaveMesh(Mesh* mesh, char** fileBuffer);
	void SaveMaterial(Material* material, char** fileBuffer);
	void SaveTexture(TextureData* texture, char** fileBuffer);

}

namespace DataLoading {

	void LoadMesh(char* fileBuffer, Mesh* mesh);
	void LoadMaterial(char* fileBuffer, Material* material);
	void LoadTexture(char* fileBuffer, TextureData* texture);

}

#endif

