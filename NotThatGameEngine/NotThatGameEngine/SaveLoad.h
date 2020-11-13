#ifndef __SAVELOAD_H__
#define __SAVELOAD_H__

#include "Globals.h"

class Component;
class Application;
class Mesh;
class Material;
class TextureData;

/* When an obect is created and its data is uptaded, those functions must be called. It's not beautiful code, but when I got here everything was already creating the structure, then filling with the data,
 thus reworking it to only call Save and Load where all data converges (once the structure is filled) would take time. Incidentally, time is exactly what I don't have. If it helps, search AddComponent() */

namespace DataSaving {

	void SaveMesh(Application* App, Mesh* mesh);
	void SaveMaterial(Application* App, Material* material);
	void SaveTexture(Application* App, TextureData* texture);

}

namespace DataLoading {

	void LoadMesh(char* fileBuffer, Mesh* mesh);
	void LoadMaterial(char* fileBuffer, Material* material);
	uint LoadTexture(Application* App, const char* fileName);

}

#endif
