#include "SaveLoad.h"
#include "Application.h"


void DataSaving::SaveMesh(Application* App, Mesh* mesh/*, char** fileBuffer*/) {

	int vertexSize = sizeof(float) * mesh->vertices.size();
	int indexSize = sizeof(uint) * mesh->indices.size();
	int normalSize = sizeof(float) * mesh->normals.size();
	int textureCoordSize = sizeof(float) * mesh->textureCoord.size();

	uint size = vertexSize + indexSize + normalSize + textureCoordSize;

	char* buffer = new char[size];
	char* cursor = buffer;


	memcpy(cursor, &mesh->vertices.at(0), vertexSize);
	cursor += vertexSize;

	memcpy(cursor, &mesh->indices.at(0), indexSize);
	cursor += indexSize;


	//Store normals
	if (mesh->normals.size() > 0) {

		memcpy(cursor, &mesh->normals.at(0), normalSize);
		cursor += normalSize;

	}

	//Store texture coordinates
	if (mesh->textureCoord.size() > 0)	{

		memcpy(cursor, &mesh->textureCoord.at(0), textureCoordSize);
		cursor += textureCoordSize;

	}

	//App->externalManager->Save(file, buffer, size);

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
}

void DataLoading::LoadMaterial(char* fileBuffer, Material* material) {
	//NOT CALLED

}

void DataLoading::LoadTexture(char* fileBuffer, TextureData* texture) {
	//NOT CALLED
}

