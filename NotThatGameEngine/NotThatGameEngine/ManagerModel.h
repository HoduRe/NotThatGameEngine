#ifndef __ManagerModel_H__
#define __ManagerModel_H__

#include "Module.h"

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

	SubMeshes();
	~SubMeshes();

	GLuint vertexId;
	GLuint indexId;
	GLuint normalsId;
	GLuint textureCoordId;
	unsigned int materialId;

	int vertexVectorSize;
	int indexVectorSize;
	int normalVectorSize;
	int textureCoordVectorSize;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> textureCoord;
	std::vector<float> colors;
	std::vector<uint> indices;
};


struct Mesh {

public:

	std::vector<SubMeshes> subMeshes = std::vector<SubMeshes>();
	std::vector<Texture*> textures;

};

class ManagerModel : public Module
{

public:

	ManagerModel(Application* app, bool start_enabled = true);
	~ManagerModel();

	bool Init();
	bool Start();
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