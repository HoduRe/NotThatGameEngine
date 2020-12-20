#include "Application.h"
#include "Load.h"
#include "Save.h"
#include "ManagerResource.h"
#include "EditorScene.h"
#include "ManagerEvent.h"
#include "ManagerExternal.h"
#include "Textures.h"
#include "TextureData.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
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

	GameObject* gameObject = new GameObject(App, ID, name, App->editorScene->FindGameObject(parentID), enabled);

	Transform* transform = (Transform*)gameObject->GetComponent(COMPONENT_TYPE::TRANSFORM);
	transform->SetPosition(float3(translationX, translationY, translationZ));
	transform->SetEulerAngles(float3(rotationX, rotationY, rotationZ));
	transform->SetScale(float3(scaleX, scaleY, scaleZ));

	App->editorScene->AddGameObject(gameObject);

	JSON_Array* componentNode(json_object_get_array(itNode, JSON_NODE_COMPONENTS));
	ComponentReader cReader;

	for (int i = 0; i < JsonManager::GetArraySize(componentNode); i++) {

		char* buffer;
		uint size;

		JSON_Object* nodeComponent = json_array_get_object(componentNode, i);
		cReader.componentType = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_TYPE);
		cReader.componentId = json_object_get_number(nodeComponent, JSON_NODE_COMPONENT_ID);

		gameObject->AddComponent((COMPONENT_TYPE)cReader.componentType, cReader.componentId);

		if (cReader.componentType == (int)COMPONENT_TYPE::MESH) {
			size = App->externalManager->Load((MESHES_PATH + std::to_string(cReader.componentId) + EXTENSION_MESHES).c_str(), &buffer);
			if (size > 0) {
				LoadMesh(buffer, gameObject->mesh);
				RELEASE_ARRAY(buffer);
			}
			else {
				int a = 1;
			}
		}

		else if (cReader.componentType == (int)COMPONENT_TYPE::MATERIAL) {
			size = App->externalManager->Load((MATERIALS_PATH + std::to_string(cReader.componentId) + EXTENSION_MATERIALS).c_str(), &buffer);
			if (size > 0) {
				LoadMaterial(App, buffer, gameObject->material);
				RELEASE_ARRAY(buffer);
			}
		}

		else if (cReader.componentType == (int)COMPONENT_TYPE::CAMERA) {
			size = App->externalManager->Load((CAMERAS_PATH + std::to_string(cReader.componentId) + EXTENSION_CAMERA).c_str(), &buffer);
			if (size > 0) {
				LoadCamera(buffer, gameObject->camera);
				RELEASE_ARRAY(buffer);
			}
		}

	}

	if (gameObject->parent == nullptr) {
		App->eventManager->GenerateEvent(EVENT_ENUM::GAMEOBJECT_LOADED, EVENT_ENUM::NULL_EVENT, gameObject);
	}

}


void DataLoading::LoadModel(Application* App, char* buffer) {

	JsonManager::JsonValue root(json_parse_string(buffer));
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));

	int size = JsonManager::GetArraySize(gameObjectsArray);

	for (int i = 0; i < size; i++) { LoadGameObject(App, gameObjectsArray, i); }

}


void DataLoading::LoadMesh(char* fileBuffer, Mesh* mesh) {

	const char* cursor = fileBuffer;

	int vertexVecSize;
	int indexVecSize;
	int normalVecSize;
	int textureCoordVecSize;
	int boneIDsSize;
	int boneNamesSize;
	int boneOffsetMatrixSize;

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

	memcpy(&mesh->boneIdsbyVertexIndexSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&mesh->weightsByVertexIndexSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&mesh->boneDisplayVecSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&boneIDsSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&boneNamesSize, cursor, sizeof(int));
	cursor += sizeof(int);

	memcpy(&boneOffsetMatrixSize, cursor, sizeof(int));
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



	mesh->vertices.clear();
	mesh->indices.clear();
	mesh->normals.clear();
	mesh->textureCoord.clear();

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
	material->SetTextureName(App, name.c_str());

}


void DataLoading::LoadCamera(char* fileBuffer, Camera* camera) {

	// TODO: well, do it

}


uint DataLoading::LoadTexture(Application* App, std::string path, const char* buffer, uint size) {

	std::string textureName, extension;
	App->externalManager->SplitFilePath(path.c_str(), nullptr, &textureName, &extension);
	uint imageTest = App->texture->IsTextureRepeated(textureName.c_str());	// This should check in memoryMap

	if (imageTest == 0) {

		ilGenImages(1, &imageTest);
		ilBindImage(imageTest);

		if (buffer == nullptr && size == 0) { size = App->externalManager->Load(path.c_str(), (char**)&buffer); }

		ILboolean ret = ilLoadL(IL_TYPE_UNKNOWN, buffer, size);

		RELEASE_ARRAY(buffer);

		if (ret != IL_TRUE) {
			LOG("Texture with path %s failed to load.\n", path);
			return 0;
		}

		OpenGLFunctionality::LoadGLTexture(&imageTest, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());

		TextureData* texture = new TextureData(imageTest, App->idGenerator.Int(), textureName.c_str(), GL_DIFFUSE, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), size);
		texture->reference++;
		App->texture->AddTexture(texture);

		ilDeleteImages(1, &imageTest);

	}

	else {
	
		App->texture->GetTextureData(imageTest)->reference++;
		LOG("Repeated texture %s.\n", textureName.c_str());
	
	}

	return imageTest;

}


void DataLoading::LoadAssetsMap(Application* App, std::map<std::string, FileInfo>* assetsMap) {

	char* buffer;
	uint size = App->externalManager->Load((LIBRARY_PATH + (std::string)ASSETS_MAP + EXTENSION_MAP).c_str(), &buffer);

	if (size != 0) {

		JsonManager::JsonValue root(json_parse_string(buffer));
		JSON_Object* node(json_value_get_object(root.value));
		JSON_Array* assetsMapArray(json_object_get_array(node, JSON_NODE_MAP_DATA));

		int size = JsonManager::GetArraySize(assetsMapArray);

		for (int i = 0; i < size; i++) {

			JSON_Object* itNode = json_array_get_object(assetsMapArray, i);
			std::string fileName = json_object_get_string(itNode, JSON_NODE_MAP_FILENAME);
			std::string filePath = json_object_get_string(itNode, JSON_NODE_MAP_FILEPATH);
			int ID = json_object_get_number(itNode, JSON_NODE_MAP_ID);
			int lastTimeChanged = json_object_get_number(itNode, JSON_NODE_MAP_LAST_CHANGE);
			assetsMap->insert(std::pair<std::string, FileInfo>(fileName, FileInfo(filePath, ID, lastTimeChanged, false)));

		}

		RELEASE_ARRAY(buffer);

	}

}


void DataLoading::LoadLibraryMap(Application* App, std::map<std::string, LibraryInfo>* libraryMap) {

	char* buffer;
	uint size = App->externalManager->Load((LIBRARY_PATH + (std::string)LIBRARY_MAP + EXTENSION_MAP).c_str(), &buffer);

	if (size != 0) {

		JsonManager::JsonValue root(json_parse_string(buffer));
		JSON_Object* node(json_value_get_object(root.value));
		JSON_Array* assetsMapArray(json_object_get_array(node, JSON_NODE_MAP_DATA));

		int size = JsonManager::GetArraySize(assetsMapArray);

		for (int i = 0; i < size; i++) {

			JSON_Object* itNode = json_array_get_object(assetsMapArray, i);
			std::string fileName = json_object_get_string(itNode, JSON_NODE_MAP_FILENAME);
			std::string filePath = json_object_get_string(itNode, JSON_NODE_MAP_FILEPATH);
			ResourceEnum type = (ResourceEnum)json_object_get_number(itNode, JSON_NODE_MAP_TYPE);
			libraryMap->insert(std::pair<std::string, LibraryInfo>(fileName, LibraryInfo(filePath, type)));

		}

		RELEASE_ARRAY(buffer);

	}

}



