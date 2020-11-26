#include "Application.h"
#include "Load.h"
#include "Save.h"
#include "ManagerResource.h"
#include "EditorScene.h"
#include "ManagerExternal.h"
#include "Textures.h"
#include "TextureData.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "parson/parson.h"

void DataLoading::LoadScene(Application* App, char* buffer) {

	App->editorScene->DeleteAllGameObjects();

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));

	int size = JsonManager::GetArraySize(gameObjectsArray);

	for (int i = 0; i < size; i++) { LoadGameObject(App, gameObjectsArray, i); }

	// TODO: Load other components (camera)

}


void DataLoading::LoadGameObject(Application* App, JSON_Array* gameObjectsArray, int index) {

	JSON_Object* itNode = json_array_get_object(gameObjectsArray, index);

	std::string name = json_object_get_string(itNode, JSON_NODE_NAME);
	long long int ID = json_object_get_number(itNode, JSON_NODE_ID);
	int parentID = json_object_get_number(itNode, JSON_NODE_PARENT_ID);

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

	bool enabled = json_object_get_boolean(itNode, JSON_NODE_ENABLED);

	GameObject* gameObject = new GameObject(ID, name, App->editorScene->FindGameObject(parentID), enabled);

	Transform* transform = (Transform*)gameObject->FindComponent(COMPONENT_TYPE::TRANSFORM);
	transform->SetPosition(float3(translationX, translationY, translationZ));
	transform->SetEulerAngles(float3(rotationX, rotationY, rotationZ));
	transform->SetScale(float3(scaleX, scaleY, scaleZ));

	App->editorScene->AddGameObject(gameObject);

	JSON_Array* componentNode(json_object_get_array(itNode, JSON_NODE_COMPONENTS));
	ComponentReader cReader;

	for (int i = 0; i < JsonManager::GetArraySize(componentNode); i++) {

		JSON_Object* nodeComponent = json_array_get_object(componentNode, i);
		cReader.componentType = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_TYPE);
		cReader.componentId = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_ID);

		gameObject->AddComponent((COMPONENT_TYPE)cReader.componentType, cReader.componentId);

		ResourceEnum type;
		if (cReader.componentType == 2) { type = ResourceEnum::MESH; }
		else if (cReader.componentType == 3) { type = ResourceEnum::MATERIAL; }

		App->resourceManager->LoadResourceByType(std::to_string(cReader.componentId), type);

	}

}


GameObject* DataLoading::LoadModel(Application* App, char* buffer) {

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));
	GameObject* gameObject = nullptr;

	int size = JsonManager::GetArraySize(gameObjectsArray);

	for (int i = 0; i < size; i++) { LoadGameObject(App, gameObjectsArray, i); }

	if (size > 0) {

		JSON_Object* itNode = json_array_get_object(gameObjectsArray, 0);
		gameObject = App->editorScene->FindGameObject(json_object_get_number(itNode, JSON_NODE_ID));

		NewGameObjectFromModel(App, gameObject);

	}

	return gameObject;

}


void DataLoading::NewGameObjectFromModel(Application* App, GameObject* gameObject) {

	long long int id = App->idGenerator.Int();
	gameObject->id = id;

	for (int i = 0; i < gameObject->components.size(); i++) {

		id = App->idGenerator.Int();
		gameObject->components[i]->id = id;

	}

	for (int i = 0; i < gameObject->childs.size(); i++) { NewGameObjectFromModel(App, gameObject->childs[i]); }

}


void DataLoading::LoadMesh(char* fileBuffer, Mesh* mesh) {

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

void DataLoading::LoadMaterial(Application* App, char* fileBuffer, Material* material) {

	char* cursor = fileBuffer;

	int nameSize;
	memcpy(&nameSize, cursor, sizeof(int));
	cursor += sizeof(int);

	std::string name;
	memcpy((void*)name.c_str(), cursor, nameSize + 1);	// I was about to write something very disturbing. Happy thoughts, happy thoughts

	material->diffuseId = App->texture->IsTextureRepeated(name.c_str());

}

uint DataLoading::LoadTexture(Application* App, std::string path, const char* buffer, uint size) {

	std::string textureName, extension;
	App->externalManager->SplitFilePath(path.c_str(), nullptr, &textureName, &extension);
	uint imageTest = App->texture->IsTextureRepeated(textureName.c_str());

	if (imageTest == 0) {

		ilGenImages(1, &imageTest);
		ilBindImage(imageTest);

		if (buffer == nullptr && size == 0) { size = App->externalManager->Load(path.c_str(), (char**)&buffer); }

		ILboolean ret = ilLoadL(IL_TYPE_UNKNOWN, buffer, size);

		RELEASE_ARRAY(buffer);

		if (ret == IL_TRUE) {

			if ("." + extension != EXTENSION_TEXTURES) {	// Always working with our .dds textures!!

				std::string pathAux = DataSaving::SaveTexture(App, textureName);
				ilDeleteImages(1, &imageTest);
				return LoadTexture(App, pathAux.c_str());

			}

			LOG("Texture with path %s loaded.\n", path);

		}

		else {
			LOG("Texture with path %s failed to load.\n", path);
			return 0;
		}

		OpenGLFunctionality::LoadGLTexture(&imageTest, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());

		TextureData* texture = new TextureData(imageTest, App->idGenerator.Int(), textureName.c_str(), GL_DIFFUSE, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), size);

		App->texture->AddTexture(texture);
		DataSaving::SaveTexture(App, texture->name);

		ilDeleteImages(1, &imageTest);

	}

	else { LOG("Repeated texture %s.\n", textureName.c_str()); }

	return imageTest;

}


