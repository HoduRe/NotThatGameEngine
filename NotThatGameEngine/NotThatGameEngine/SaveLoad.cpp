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

	if (mesh->vertices.size() > 0) {

		memcpy(cursor, &mesh->vertices.at(0), vertexSize);
		cursor += vertexSize;

	}

	if (mesh->indices.size() > 0) {

		memcpy(cursor, &mesh->indices.at(0), indexSize);
		cursor += indexSize;

	}

	if (mesh->normals.size() > 0) {

		memcpy(cursor, &mesh->normals.at(0), normalSize);
		cursor += normalSize;

	}

	if (mesh->textureCoord.size() > 0) {

		memcpy(cursor, &mesh->textureCoord.at(0), textureCoordSize);
		cursor += textureCoordSize;

	}

	std::string path = (std::string)MESHES_PATH + std::to_string(mesh->id) + EXTENSION_MESHES;

	App->externalManager->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}

void DataSaving::SaveMaterial(Application* App, Material* material) {

	uint size = sizeof(uint);

	char* buffer = new char[size];

	memcpy(buffer, &material->diffuseId, size);

	std::string path = (std::string)MATERIALS_PATH + std::to_string(material->id) + EXTENSION_MATERIALS;

	App->externalManager->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}

void DataSaving::SaveTexture(Application* App, TextureData* texture) {	// Called everytime we load a texture to memory; no need to call it when saving scene (if it has to be called from somewhere else, remember to Bind the texture for DevIL)

	char* buffer;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	ILuint size = ilSaveL(IL_DDS, nullptr, 0);
	ILubyte* data = new ILubyte[size];

	if (size > 0) {

		if (ilSaveL(IL_DDS, data, size) > 0) {

			std::string path = (std::string)TEXTURES_PATH + texture->name + EXTENSION_TEXTURES;
			App->externalManager->Save(path.c_str(), data, size);

		}

	}

	RELEASE_ARRAY(data);

}


void DataSaving::SaveGameObject(Application* App, JSON_Object* node, GameObject* gameObject) {

	json_object_set_string(node, JSON_NODE_NAME, gameObject->name.c_str());
	json_object_set_number(node, JSON_NODE_ID, gameObject->id);
	json_object_set_number(node, JSON_NODE_PARENT_ID, gameObject->parent ? gameObject->parent->id : 0);

	Transform* transform = (Transform*)gameObject->FindComponent(COMPONENT_TYPE::TRANSFORM);

	JSON_Array* jsonObject(JsonManager::OpenArray(node, JSON_NODE_TRANSLATION));
	json_array_append_number(jsonObject, transform->GetPosition().x);
	json_array_append_number(jsonObject, transform->GetPosition().x);
	json_array_append_number(jsonObject, transform->GetPosition().x);
	jsonObject = nullptr;

	jsonObject = JsonManager::OpenArray(node, JSON_NODE_ROTATION);
	json_array_append_number(jsonObject, transform->GetEulerAngles().x);
	json_array_append_number(jsonObject, transform->GetEulerAngles().y);
	json_array_append_number(jsonObject, transform->GetEulerAngles().z);
	jsonObject = nullptr;

	jsonObject = JsonManager::OpenArray(node, JSON_NODE_SCALE);
	json_array_append_number(jsonObject, transform->GetScale().x);
	json_array_append_number(jsonObject, transform->GetScale().y);
	json_array_append_number(jsonObject, transform->GetScale().z);
	jsonObject = nullptr;

	// TODO: Why don't you save if object is enabled or not, prick

	JSON_Array* gameComponentsArray(JsonManager::OpenArray(node, JSON_NODE_COMPONENTS));
	const std::vector<Component*> components = gameObject->components;

	for (uint i = 0; i < components.size(); i++) {

		if (components[i]->type != COMPONENT_TYPE::TRANSFORM) {

			json_array_append_value(gameComponentsArray, json_value_init_object());
			JSON_Object* nodeComponent = json_array_get_object(gameComponentsArray, i - 1);
			// The i-1 is ugly, but since Transforms are mandatory, and they are created as the GameObject is initialized, it's probably safe

			DataSaving::SaveComponent(App, components[i]);

			json_object_set_number(nodeComponent, JSON_NODE_COMPONENT_TYPE, (int)components[i]->type);
			json_object_set_number(nodeComponent, JSON_NODE_COMPONENT_ID, components[i]->id);

		}

	}

}


void DataSaving::SaveComponent(Application* App, Component* component) {

	switch (component->type) {

	case COMPONENT_TYPE::MESH:

		DataSaving::SaveMesh(App, (Mesh*)component);

		break;

	case COMPONENT_TYPE::MATERIAL:

		DataSaving::SaveMaterial(App, (Material*)component);

		break;

	default:

		break;

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

uint DataLoading::LoadTexture(Application* App, const char* path) { return DataImporter::LoadTexture(App, path); }


void DataLoading::LoadGameObject(Application* App, JSON_Array* gameObjectsArray, int index) {

	JSON_Object* itNode = json_array_get_object(gameObjectsArray, index);

	std::string name = json_object_get_string(itNode, JSON_NODE_NAME);
	long long int ID = json_object_get_number(itNode, JSON_NODE_ID);
	int parentID = json_object_get_number(itNode, JSON_NODE_NAME);

	JSON_Array* transformNode(json_object_get_array(itNode, JSON_NODE_TRANSLATION));
	float translationX = json_value_get_number(json_array_get_value(transformNode, 0));
	float translationY = json_value_get_number(json_array_get_value(transformNode, 1));
	float translationZ = json_value_get_number(json_array_get_value(transformNode, 2));

	transformNode = json_object_get_array(itNode, JSON_NODE_ROTATION);
	float rotationX = json_value_get_number(json_array_get_value(transformNode, 0));
	float rotationY = json_value_get_number(json_array_get_value(transformNode, 1));
	float rotationZ = json_value_get_number(json_array_get_value(transformNode, 2));

	transformNode = json_object_get_array(itNode, JSON_NODE_SCALE);
	float scaleX = json_value_get_number(json_array_get_value(transformNode, 0));
	float scaleY = json_value_get_number(json_array_get_value(transformNode, 1));
	float scaleZ = json_value_get_number(json_array_get_value(transformNode, 2));

	GameObject* gameObject = new GameObject(ID, name, App->editorScene->FindGameObject(parentID), true);	// TODO: when I load bool enabled, this true should be a variable

	Transform* transform = (Transform*)gameObject->FindComponent(COMPONENT_TYPE::TRANSFORM);
	transform->SetPosition(float3(translationX, translationY, translationZ));
	transform->SetEulerAngles(float3(rotationX, rotationY, rotationZ));
	transform->SetScale(float3(scaleX, scaleY, scaleZ));

	JSON_Array* componentNode(json_object_get_array(itNode, JSON_NODE_COMPONENTS));
	ComponentReader cReader;

	for (int i = 0; i < JsonManager::GetArraySize(componentNode); i++) {

		JSON_Object* nodeComponent = json_array_get_object(componentNode, i);
		cReader.componentType = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_TYPE);
		cReader.componentId = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_ID);

		gameObject->AddComponent((COMPONENT_TYPE)cReader.componentType, cReader.componentId);

	}

	App->editorScene->AddGameObject(gameObject);

}





