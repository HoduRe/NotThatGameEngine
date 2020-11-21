#include "Load.h"
#include "Application.h"
#include "Save.h"

void DataLoading::LoadScene(Application* App, char* buffer) {

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));

	int size = JsonManager::GetArraySize(gameObjectsArray);

	for (int i = 0; i < size; i++) { LoadGameObject(App, gameObjectsArray, i); }

}


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

	bool enabled = json_object_get_boolean(itNode, JSON_NODE_ENABLED);
	std::string originalName = json_object_get_string(itNode, JSON_NODE_MODEL_NAME);

	GameObject* gameObject = new GameObject(ID, originalName, name, App->editorScene->FindGameObject(parentID), enabled);

	Transform* transform = (Transform*)gameObject->FindComponent(COMPONENT_TYPE::TRANSFORM);
	transform->SetPosition(float3(translationX, translationY, translationZ));
	transform->SetEulerAngles(float3(rotationX, rotationY, rotationZ));
	transform->SetScale(float3(scaleX, scaleY, scaleZ));

	LoadModel(App, gameObject);

	App->editorScene->AddGameObject(gameObject);

}


void DataLoading::LoadModel(Application* App, GameObject* gameObject) {

	char* buffer;
	App->externalManager->Load((MODELS_PATH + gameObject->originalName + EXTENSION_MODELS).c_str(), &buffer);

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* componentNode(json_object_get_array(node, JSON_NODE_COMPONENTS));
	ComponentReader cReader;

	for (int i = 0; i < JsonManager::GetArraySize(componentNode); i++) {

		JSON_Object* nodeComponent = json_array_get_object(componentNode, i);
		cReader.componentType = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_TYPE);
		cReader.componentId = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_ID);

		gameObject->AddComponent((COMPONENT_TYPE)cReader.componentType, cReader.componentId);

	}

	RELEASE_ARRAY(buffer);

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

uint DataLoading::LoadTexture(Application* App, const char* path, const char* buffer, uint size) {

	std::string textureName, extension;
	App->externalManager->SplitFilePath(path, nullptr, &textureName, &extension);
	uint imageTest = App->texture->IsTextureRepeated(textureName.c_str());

	if (imageTest == 0) {

		ilGenImages(1, &imageTest);
		ilBindImage(imageTest);

		if (buffer == nullptr && size == 0) { size = App->externalManager->Load(path, (char**)&buffer); }

		ILboolean ret = ilLoadL(IL_TYPE_UNKNOWN, buffer, size);

		RELEASE_ARRAY(buffer);

		if (ret == IL_TRUE) {

			if ("." + extension != EXTENSION_TEXTURES) {	// Always working with our .dds textures!!

				std::string path = DataSaving::SaveTexture(App, textureName);
				ilDeleteImages(1, &imageTest);
				return LoadTexture(App, path.c_str());

			}

			LOG("Texture with path %s loaded.\n", path);

		}

		else {
			LOG("Texture with path %s failed to load.\n", path);
			return 0;
		}

		OpenGLFunctionality::LoadGLTexture(&imageTest, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());

		TextureData* texture = new TextureData(imageTest, textureName.c_str(), GL_DIFFUSE, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), size);

		App->texture->AddTexture(texture);
		DataSaving::SaveTexture(App, texture->name);	// TODO: so if I'm loading an imported DDS texture, I save it, but it's the same... I don't have editable textures, so why? Maybe this is solved by having a timer for resource manager?

		ilDeleteImages(1, &imageTest);

	}

	return imageTest;

}


