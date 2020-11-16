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

	std::string path = (std::string)MESHES_PATH + mesh->meshName + ".NotThatExtension";	// TODO: it would be cool to have this be the UUID, then when an object is loaded with its components, it simply has to call the load function by UUID, and the functions uses that name no problem

	App->externalManager->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}

void DataSaving::SaveMaterial(Application* App, Material* material) {
	int a = 2;

}

void DataSaving::SaveTexture(Application* App, TextureData* texture) {

	char* buffer;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	ILuint size = ilSaveL(IL_DDS, nullptr, 0);
	ILubyte* data = new ILubyte[size];

	if (size > 0) {

		if (ilSaveL(IL_DDS, data, size) > 0) {

			std::string path = (std::string)TEXTURES_PATH + texture->name + ".dds";
			App->externalManager->Save(path.c_str(), data, size);

		}

	}

	RELEASE_ARRAY(data);

}


void DataSaving::SaveGameObject(Application* App, JSON_Object* node, GameObject* gameObject) {

	json_object_set_string(node, "Name", gameObject->name.c_str());
	json_object_set_number(node, "ID", gameObject->id);
	json_object_set_number(node, "ParentID", gameObject->parent ? gameObject->parent->id : 0);

	Transform* transform = (Transform*)gameObject->FindComponent(COMPONENT_TYPE::TRANSFORM);

	JSON_Array* jsonObject = JsonManager::OpenArray(node, "Translation");
	json_array_append_number(jsonObject, transform->GetPosition().x);
	json_array_append_number(jsonObject, transform->GetPosition().x);
	json_array_append_number(jsonObject, transform->GetPosition().x);
	
	jsonObject = JsonManager::OpenArray(node, "Rotation");
	json_array_append_number(jsonObject, transform->GetEulerAngles().x);
	json_array_append_number(jsonObject, transform->GetEulerAngles().y);
	json_array_append_number(jsonObject, transform->GetEulerAngles().z);

	jsonObject = JsonManager::OpenArray(node, "Scale");
	json_array_append_number(jsonObject, transform->GetScale().x);
	json_array_append_number(jsonObject, transform->GetScale().y);
	json_array_append_number(jsonObject, transform->GetScale().z);

	JSON_Array* gameObjectsArray = JsonManager::OpenArray(node, "Components");
	const std::vector<Component*> components = gameObject->components;

	for (uint i = 0; i < components.size(); i++) {

		json_object_set_number(node, "ComponentType", (int)components[i]->type);
		json_object_set_number(node, "ID", components[i]->id);
		
	}

}


void DataLoading::LoadMesh(char* fileBuffer, Mesh* mesh) {
	//NOT CALLED

	const char* cursor = fileBuffer;

	int vertexVecSize;
	int indexVecSize;
	int normalVecSize;
	int textureCoordVecSize;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> textureCoord;
	std::vector<GLuint> indices;

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
		vertices.push_back(data);
		cursor += sizeof(float);

	}

	for (int i = 0; i < indexVecSize; i++) {

		uint data;
		memcpy(&data, cursor, sizeof(uint));
		indices.push_back(data);
		cursor += sizeof(uint);

	}

	for (int i = 0; i < normalVecSize; i++) {

		float data;
		memcpy(&data, cursor, sizeof(float));
		normals.push_back(data);
		cursor += sizeof(float);

	}

	for (int i = 0; i < textureCoordVecSize; i++) {

		float data;
		memcpy(&data, cursor, sizeof(float));
		textureCoord.push_back(data);
		cursor += sizeof(float);

	}

	mesh->SetVertices(vertices);
	mesh->SetNormals(normals);
	mesh->SetTextureCoord(textureCoord);
	mesh->SetIndices(indices);

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


void LoadGameObject(Application* App, GameObject* gameObject) {



}


