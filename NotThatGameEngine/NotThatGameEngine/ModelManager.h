#ifndef __MODELMANAGER_H__
#define __MODELMANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "Primitives.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/postprocess.h"


struct Texture
{
	Texture(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

	std::string fileName;
	GLenum textureTarget;
	GLuint textureId;
};


struct SubMeshes {

public:

	SubMeshes() {};

	GLuint vertexId = 0;
	GLuint indexId = 0;
	unsigned int numIndices = 0;
	unsigned int materialId = 0;

	int vertexVectorSize = 0;
	int indexVectorSize = 0;

	std::vector<float> vertices;
	std::vector<uint> indices;
};


struct Mesh {

public:

	std::vector<SubMeshes> subMeshes;
	std::vector<Texture*> textures;

};

class ModelManager : public Module
{

public:

	ModelManager(Application* app, bool start_enabled = true);
	~ModelManager();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadModel(std::string path);

public:

private:

	void ExecuteEvent(EVENT_ENUM eventId);

private:
	aiLogStream stream;
	Mesh testMesh;
};

#endif