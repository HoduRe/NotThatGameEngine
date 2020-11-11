#include "SaveLoad.h"
#include "Application.h"


void DataSaving::SaveMesh(Mesh* mesh, char** fileBuffer) {

}

void DataSaving::SaveMaterial(Material* material, char** fileBuffer) {

}

void DataSaving::SaveTexture(TextureData* texture, char** fileBuffer) {
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

