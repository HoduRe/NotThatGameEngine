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
#include "Animation.h"
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
		}

		else if (cReader.componentType == (int)COMPONENT_TYPE::MATERIAL) {
			size = App->externalManager->Load((MATERIALS_PATH + std::to_string(cReader.componentId) + EXTENSION_MATERIALS).c_str(), &buffer);
			if (size > 0) {
				LoadMaterial(App, buffer, gameObject->material);
				RELEASE_ARRAY(buffer);
			}
		}

		else if (cReader.componentType == (int)COMPONENT_TYPE::ANIMATION) {
			size = App->externalManager->Load((ANIMATIONS_PATH + std::to_string(cReader.componentId) + EXTENSION_ANIMATIONS).c_str(), &buffer);
			if (size > 0) {
				LoadAnimation(buffer, gameObject->animation);
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

	RELEASE_ARRAY(mesh->vertices);
	RELEASE_ARRAY(mesh->indices);
	RELEASE_ARRAY(mesh->normals);
	RELEASE_ARRAY(mesh->textureCoord);

	const char* cursor = fileBuffer;

	int intSize = sizeof(int);
	int vertexVecSize;
	int indexVecSize;
	int normalVecSize;
	int textureCoordVecSize;
	int boneNamesSize;
	int boneOffsetMatrixSize;

	memcpy(&vertexVecSize, cursor, intSize);
	cursor += intSize;
	mesh->vertexSize = vertexVecSize;

	memcpy(&indexVecSize, cursor, intSize);
	cursor += intSize;
	mesh->indexSize = indexVecSize;

	memcpy(&normalVecSize, cursor, intSize);
	cursor += intSize;
	mesh->normalsSize = normalVecSize;

	memcpy(&textureCoordVecSize, cursor, intSize);
	cursor += intSize;
	mesh->textureCoordSize = textureCoordVecSize;

	memcpy(&mesh->boneIDsSize, cursor, intSize);
	cursor += intSize;

	memcpy(&mesh->boneWeightsSize, cursor, intSize);
	cursor += intSize;

	memcpy(&mesh->boneDisplayVecSize, cursor, intSize);
	cursor += intSize;

	memcpy(&boneNamesSize, cursor, intSize);
	cursor += intSize;

	memcpy(&boneOffsetMatrixSize, cursor, intSize);
	cursor += intSize;

	mesh->vertices = new float[mesh->vertexSize];
	memcpy(mesh->vertices, cursor, mesh->vertexSize * sizeof(float));
	cursor += mesh->vertexSize * sizeof(float);

	mesh->indices = new GLuint[mesh->indexSize];
	memcpy(mesh->indices, cursor, mesh->indexSize * sizeof(uint));
	cursor += mesh->indexSize * sizeof(uint);

	mesh->normals = new float[mesh->normalsSize];
	memcpy(mesh->normals, cursor, mesh->normalsSize * sizeof(float));
	cursor += mesh->normalsSize * sizeof(float);

	mesh->textureCoord = new float[mesh->textureCoordSize];
	memcpy(mesh->textureCoord, cursor, mesh->textureCoordSize * sizeof(float));
	cursor += mesh->textureCoordSize * sizeof(float);

	if (mesh->boneIDsSize > 0) {

		mesh->boneIDs = new int[mesh->boneIDsSize];
		memcpy(mesh->boneIDs, cursor, intSize * mesh->boneIDsSize);
		cursor += intSize * mesh->boneIDsSize;

	}

	if (mesh->boneWeightsSize > 0) {

		mesh->boneWeights = new float[mesh->boneWeightsSize];
		memcpy(mesh->boneWeights, cursor, sizeof(float) * mesh->boneWeightsSize);
		cursor += sizeof(float) * mesh->boneWeightsSize;

	}

	if (mesh->boneDisplayVecSize > 0) {

		mesh->boneDisplayVec = new bool[mesh->boneDisplayVecSize];
		memcpy(mesh->boneDisplayVec, cursor, sizeof(bool) * mesh->boneDisplayVecSize);
		cursor += sizeof(bool) * mesh->boneDisplayVecSize;

	}

	for (int i = 0; i < boneNamesSize; i++) {

		int nameSize;
		memcpy(&nameSize, cursor, intSize);
		cursor += intSize;

		char* data = new char[nameSize];
		memcpy(data, cursor, nameSize);
		mesh->boneNamesVec.push_back(data);
		cursor += nameSize;

	}

	for (int i = 0; i < boneOffsetMatrixSize; i++) {

		float4x4 data;
		memcpy(&data, cursor, sizeof(float4x4));
		mesh->boneOffsetMatrixVec.push_back(data);
		cursor += sizeof(float4x4);

	}

	mesh->SetVertices(nullptr);
	mesh->SetNormals(nullptr);
	mesh->SetTextureCoord(nullptr);
	mesh->SetIndices(nullptr);

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


void DataLoading::LoadAnimation(char* fileBuffer, Animation* animation) {

	char* cursor = fileBuffer;
	int intSize = sizeof(int);
	int floatSize = sizeof(float);
	int boolSize = sizeof(bool);

	int animationVecSize, nameSize, mapSize;

	char* name;
	float duration;
	float ticksPerSecond;
	int channelsAmount;
	bool playing;

	float time, x, y, z, w;

	memcpy(&animationVecSize, cursor, intSize);
	cursor += intSize;

	for (int i = 0; i < animationVecSize; i++) {

		memcpy(&nameSize, cursor, intSize);
		cursor += intSize;

		name = new char[nameSize];
		memcpy(name, cursor, nameSize);
		cursor += nameSize;

		memcpy(&duration, cursor, floatSize);
		cursor += floatSize;

		memcpy(&ticksPerSecond, cursor, floatSize);
		cursor += floatSize;

		memcpy(&channelsAmount, cursor, intSize);
		cursor += intSize;

		memcpy(&playing, cursor, boolSize);
		cursor += boolSize;

		animation->animationVec.push_back(AnimationData(name, duration, ticksPerSecond, playing));

		for (int j = 0; j < channelsAmount; j++) {

			memcpy(&nameSize, cursor, intSize);
			cursor += intSize;

			name = new char[nameSize];
			memcpy(name, cursor, nameSize);
			cursor += nameSize;

			animation->animationVec[i].channels.insert(std::pair <std::string, Channels>(name, Channels()));
			std::map<std::string, Channels>::iterator channelIt = prev(animation->animationVec[i].channels.end());

			memcpy(&mapSize, cursor, intSize);
			cursor += intSize;

			for (int p = 0; p < mapSize; p++) {

				memcpy(&time, cursor, floatSize);
				cursor += floatSize;

				memcpy(&x, cursor, floatSize);
				cursor += floatSize;

				memcpy(&y, cursor, floatSize);
				cursor += floatSize;

				memcpy(&z, cursor, floatSize);
				cursor += floatSize;

				channelIt->second.positionKeys.insert(std::pair<float, float3>(time, float3(x, y, z)));

			}

			memcpy(&mapSize, cursor, intSize);
			cursor += intSize;

			for (int r = 0; r < mapSize; r++) {

				memcpy(&time, cursor, floatSize);
				cursor += floatSize;

				memcpy(&x, cursor, floatSize);
				cursor += floatSize;

				memcpy(&y, cursor, floatSize);
				cursor += floatSize;

				memcpy(&z, cursor, floatSize);
				cursor += floatSize;

				memcpy(&w, cursor, floatSize);
				cursor += floatSize;

				channelIt->second.rotationKeys.insert(std::pair<float, Quat>(time, Quat(x, y, z, w)));

			}

			memcpy(&mapSize, cursor, intSize);
			cursor += intSize;

			for (int s = 0; s < mapSize; s++) {

				memcpy(&time, cursor, floatSize);
				cursor += floatSize;

				memcpy(&x, cursor, floatSize);
				cursor += floatSize;

				memcpy(&y, cursor, floatSize);
				cursor += floatSize;

				memcpy(&z, cursor, floatSize);
				cursor += floatSize;

				channelIt->second.scaleKeys.insert(std::pair<float, float3>(time, float3(x, y, z)));

			}

		}

	}

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



