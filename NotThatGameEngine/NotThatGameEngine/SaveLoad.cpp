#include "SaveLoad.h"
#include "Application.h"
#include "Importer.h"

void DataSaving::SaveMesh(Application* App, Mesh* mesh) {

	int intSize = sizeof(int);
	int vertexSize = sizeof(float) * mesh->vertices.size();
	int indexSize = sizeof(uint) * mesh->indices.size();
	int normalSize = sizeof(float) * mesh->normals.size();
	int textureCoordSize = sizeof(float) * mesh->textureCoord.size();

	uint size = intSize + intSize + intSize + intSize + vertexSize + indexSize + normalSize + textureCoordSize;

	// Add sizes to buffer

	char* buffer = new char[size];
	char* cursor = buffer;

	int vecSize = mesh->vertices.size();
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->indices.size();
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->normals.size();
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->textureCoord.size();
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	// TODO: not all primitives have indices...
	memcpy(cursor, &mesh->vertices.at(0), vertexSize);
	cursor += vertexSize;

	memcpy(cursor, &mesh->indices.at(0), indexSize);
	cursor += indexSize;

	if (mesh->normals.size() > 0) {

		memcpy(cursor, &mesh->normals.at(0), normalSize);
		cursor += normalSize;

	}

	if (mesh->textureCoord.size() > 0) {

		memcpy(cursor, &mesh->textureCoord.at(0), textureCoordSize);
		cursor += textureCoordSize;

	}

	std::string path = "Assets/Library/Meshes/" + mesh->meshName + ".NotThatExtension";	// TODO: it would be cool to have this be the UUID, then when an object is loaded with its components, it simply has to call the load function by UUID, and the functions uses that name no problem

	App->externalManager->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}

void DataSaving::SaveMaterial(Application* App, Material* material) {
	int a = 2;

}

void DataSaving::SaveTexture(Application* App, TextureData* texture) {

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	ILuint size = ilSaveL(IL_DDS, nullptr, texture->textureId);
	ILubyte* data = new ILubyte[size];

	std::string path = "Assets/Library/Textures/" +  texture->name + ".DDS";

	App->externalManager->Save(path.c_str(), data, size);

	RELEASE_ARRAY(data);

}

void DataLoading::LoadMesh(char* fileBuffer, Mesh* mesh) {
	//NOT CALLED

	const char* cursor = fileBuffer;

	int vertexVecSize;
	int indexVecSize;
	int normalVecSize;
	int textureCoordVecSize;

	memcpy(&vertexVecSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&indexVecSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&normalVecSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&textureCoordVecSize, cursor, sizeof(int));
	cursor += sizeof(int);

	for (int i = 0; i < vertexVecSize; i++) {

		float data;
		memcpy(&data, cursor, sizeof(float));
		mesh->vertices.push_back(data);
		cursor += sizeof(float);

	}

	for (int i = 0; i < indexVecSize; i++) {

		uint data;
		memcpy(&data, cursor, sizeof(uint));
		mesh->indices.push_back(data);
		cursor += sizeof(uint);

	}

	for (int i = 0; i < normalVecSize; i++) {

		float data;
		memcpy(&data, cursor, sizeof(float));
		mesh->normals.push_back(data);
		cursor += sizeof(float);

	}

	for (int i = 0; i < textureCoordVecSize; i++) {

		float data;
		memcpy(&data, cursor, sizeof(float));
		mesh->textureCoord.push_back(data);
		cursor += sizeof(float);

	}

	//TODO: We now need to upload data to OpenGL in memory. Do it here, or wherever it's needed to

}

void DataLoading::LoadMaterial(char* fileBuffer, Material* material) {
	//NOT CALLED

}

uint DataLoading::LoadTexture(Application* App, const char* fileName) {
	//NOT CALLED
	std::string textureName = TEXTURES_PATH + (std::string)fileName;
	return DataImporter::LoadTexture(App, textureName.c_str());
	// TODO: if resource manager begins after textures, we will load Alex.png and company, and then overwrite the DDS. Which is not wrong, but...
}

