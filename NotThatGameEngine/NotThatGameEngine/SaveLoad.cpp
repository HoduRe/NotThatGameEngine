#include "SaveLoad.h"
#include "Application.h"


void DataSaving::SaveMesh(Application* App, Mesh* mesh/*, char** fileBuffer*/) {

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

void DataSaving::SaveMaterial(Application* App, Material* material/*, char** fileBuffer*/) {
	int a = 2;

}

void DataSaving::SaveTexture(Application* App, TextureData* texture, char** fileBuffer) {
	int a = 2;
	// THIS SHOULD ONLY BE CREATING A DDS, SO MAYBE RECIEVEING TEXTUREDATA IS NOT NEEDED... EXCEPT FOR UUIDS?
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

void DataLoading::LoadTexture(char* fileBuffer, TextureData* texture) {
	//NOT CALLED
}

