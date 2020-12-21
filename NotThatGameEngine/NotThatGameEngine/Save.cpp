#include "Save.h"
#include "Application.h"
#include "EditorScene.h"
#include "ManagerExternal.h"
#include "ManagerResource.h"
#include "Textures.h"
#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Animation.h"
#include "parson/parson.h"

std::string DataSaving::SaveScene(Application* App) {

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(JsonManager::OpenArray(node, JSON_NODE_GAMEOBJECTS));

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;

	for (int i = 0; i < gameObjects.size(); i++) { RecursiveChildCall(App, gameObjectsArray, gameObjects[i]); }

	// TODO: Save cameras, lights...

	char* buffer = new char[JsonManager::GetArraySize(gameObjectsArray)];
	uint size = JsonManager::Serialize(root.value, &buffer);

	std::string sceneName = SCENES_PATH + (std::string)"Scene" + EXTENSION_SCENES;
	App->externalManager->Save(sceneName.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

	return sceneName;

}


void DataSaving::RecursiveChildCall(Application* App, JSON_Array* gameObjectsArray, GameObject* gameObject) {

	SaveGameObject(App, JsonManager::AddArrayNode(gameObjectsArray), gameObject);
	for (int j = 0; j < gameObject->childs.size(); j++) { RecursiveChildCall(App, gameObjectsArray, gameObject->childs[j]); }

}


void DataSaving::SaveGameObject(Application* App, JSON_Object* node, GameObject* gameObject) {

	json_object_set_string(node, JSON_NODE_NAME, gameObject->name.c_str());
	json_object_set_number(node, JSON_NODE_ID, gameObject->id);
	json_object_set_number(node, JSON_NODE_PARENT_ID, gameObject->parent ? gameObject->parent->id : 0);

	Transform* transform = (Transform*)gameObject->GetComponent(COMPONENT_TYPE::TRANSFORM);
	transform->RecalculateEulerAngles();

	JSON_Array* jsonObject(JsonManager::OpenArray(node, JSON_NODE_TRANSLATION));
	json_array_append_number(jsonObject, transform->GetPosition().x);
	json_array_append_number(jsonObject, transform->GetPosition().y);
	json_array_append_number(jsonObject, transform->GetPosition().z);
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

	json_object_set_boolean(node, JSON_NODE_ENABLED, gameObject->enabled);

	JSON_Array* gameComponentsArray(JsonManager::OpenArray(node, JSON_NODE_COMPONENTS));

	int i = 0;

	if (gameObject->mesh != nullptr) { SaveComponentJSON(App, gameComponentsArray, gameObject->mesh, i); i++; }
	if (gameObject->material != nullptr) { SaveComponentJSON(App, gameComponentsArray, gameObject->material, i); i++; }
	if (gameObject->animation != nullptr) { SaveComponentJSON(App, gameComponentsArray, gameObject->animation, i); i++; }
	if (gameObject->camera != nullptr) { SaveComponentJSON(App, gameComponentsArray, gameObject->camera, i); i++; }

}


std::string DataSaving::SaveModel(Application* App, GameObject* gameObject, std::string modelName) {

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameComponentsArray(JsonManager::OpenArray(node, JSON_NODE_GAMEOBJECTS));

	std::string nameSave = gameObject->name;
	gameObject->name = modelName;

	RecursiveChildCall(App, gameComponentsArray, gameObject);

	gameObject->name = nameSave;

	char* buffer = new char[JsonManager::GetArraySize(gameComponentsArray)];
	uint size = JsonManager::Serialize(root.value, &buffer);

	std::string sceneName = MODELS_PATH + modelName + EXTENSION_MODELS;
	App->externalManager->Save(sceneName.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

	return sceneName;

}


void DataSaving::SaveMesh(Application* App, Mesh* mesh) {

	int intSize = sizeof(int);
	int vertexSize = sizeof(float) * mesh->vertices.size();
	int indexSize = sizeof(uint) * mesh->indices.size();
	int normalSize = sizeof(float) * mesh->normals.size();
	int textureCoordSize = sizeof(float) * mesh->textureCoord.size();
	int boneIdsArraySize = sizeof(int) * mesh->boneIDsSize;
	int weightsSize = sizeof(float) * mesh->boneWeightsSize;
	int boneDisplaySize = sizeof(bool) * mesh->boneDisplayVecSize;
	int boneNamesSize = 0;
	std::vector<int> boneNamesVec;
	int boneOffsetSize = sizeof(float) * 16 * mesh->boneOffsetMatrixVec.size();

	for (int i = 0; i < mesh->boneNamesVec.size(); i++) {

		int nameSize = sizeof(char) * mesh->boneNamesVec[i].size() + 2;
		boneNamesSize += nameSize + sizeof(int);
		boneNamesVec.push_back(nameSize);

	}

	uint size = (intSize * 9) + vertexSize + indexSize + normalSize + textureCoordSize +
		boneIdsArraySize + weightsSize + boneDisplaySize + boneNamesSize + boneOffsetSize;

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

	vecSize = mesh->boneIDsSize;
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->boneWeightsSize;
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->boneDisplayVecSize;
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->boneNamesVec.size();
	memcpy(cursor, &vecSize, intSize);
	cursor += intSize;

	vecSize = mesh->boneOffsetMatrixVec.size();
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

	if (boneIdsArraySize > 0) {

		memcpy(cursor, mesh->boneIDs, boneIdsArraySize);
		cursor += boneIdsArraySize;

	}

	if (weightsSize > 0) {

		memcpy(cursor, mesh->boneWeights, weightsSize);
		cursor += weightsSize;

	}

	if (boneDisplaySize > 0) {

		memcpy(cursor, mesh->boneDisplayVec, boneDisplaySize);
		cursor += boneDisplaySize;

	}

	for (int i = 0; i < mesh->boneNamesVec.size(); i++) {

		int charsize = boneNamesVec[i];
		memcpy(cursor, &charsize, intSize);
		cursor += intSize;

		memcpy(cursor, (mesh->boneNamesVec[i] + "\0").c_str(), charsize);
		cursor += charsize;

	}

	for (int i = 0; i < mesh->boneOffsetMatrixVec.size(); i++) {

		memcpy(cursor, mesh->boneOffsetMatrixVec[i].ptr(), sizeof(float) * 16);
		cursor += sizeof(float) * 16;

	}

	std::string path = (std::string)MESHES_PATH + std::to_string(mesh->id) + EXTENSION_MESHES;

	App->externalManager->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}

void DataSaving::SaveMaterial(Application* App, Material* material) {

	std::string path = (std::string)MATERIALS_PATH + std::to_string(material->id) + EXTENSION_MATERIALS;
	char* buffer = nullptr;
	uint bufferSize;
	int nameSize;

	nameSize = material->GetTextureName().size();
	bufferSize = material->GetTextureName().size() + sizeof(int);

	buffer = new char[bufferSize];
	char* cursor = buffer;

	memcpy(cursor, &nameSize, sizeof(int));
	cursor += sizeof(int);

	memcpy(cursor, material->GetTextureName().c_str(), nameSize);

	App->externalManager->Save(path.c_str(), buffer, bufferSize);

	RELEASE_ARRAY(buffer);

}

std::string DataSaving::SaveTexture(Application* App, std::string textureName) {	// Called everytime we load a texture to memory; no need to call it when saving scene (if it has to be called from somewhere else, remember to Bind the texture for DevIL)

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	ILuint size = ilSaveL(IL_DDS, nullptr, 0);
	std::string path;

	if (size > 0) {

		ILubyte* data = new ILubyte[size];

		if (ilSaveL(IL_DDS, data, size) > 0) {

			path = (std::string)TEXTURES_PATH + textureName + EXTENSION_TEXTURES;
			App->externalManager->Save(path.c_str(), data, size);

		}

		RELEASE_ARRAY(data);
	}

	return path;

}


void DataSaving::SaveAnimation(Application* App, Animation* animation) {

	std::string path = (std::string)ANIMATIONS_PATH + std::to_string(animation->id) + EXTENSION_ANIMATIONS;
	char* buffer = nullptr;
	uint bufferSize = 0;
	int sizeFloat = sizeof(float);
	int sizeInt = sizeof(int);
	int sizeBool = sizeof(bool);
	int varSize;

	bufferSize += sizeInt;

	for (int i = 0; i < animation->animationVec.size(); i++) {

		char* name;
		bufferSize += sizeInt;
		bufferSize += sizeof(char) * animation->animationVec[i].name.size() + 2;
		bufferSize += (sizeFloat * 2);
		bufferSize += sizeInt;
		bufferSize += sizeBool;

		for (int j = 0; j < animation->animationVec[i].channels.size(); j++) {

			bufferSize += sizeInt * 4;
			bufferSize += sizeof(char) * animation->animationVec[i].channels[j].name.size() + 2;
			bufferSize += (sizeFloat * 4 * animation->animationVec[i].channels[j].positionKeys.size());
			bufferSize += (sizeFloat * 5 * animation->animationVec[i].channels[j].rotationKeys.size());
			bufferSize += (sizeFloat * 4 * animation->animationVec[i].channels[j].scaleKeys.size());

		}

	}

	buffer = new char[bufferSize];
	char* cursor = buffer;

	varSize = animation->animationVec.size();
	memcpy(cursor, &varSize, sizeInt);
	cursor += sizeInt;

	for (int i = 0; i < animation->animationVec.size(); i++) {

		varSize = sizeof(char) * animation->animationVec[i].name.size() + 2;
		memcpy(cursor, &varSize, sizeInt);
		cursor += sizeInt;

		memcpy(cursor, (animation->animationVec[i].name + "\0").c_str(), varSize);
		cursor += varSize;

		memcpy(cursor, &animation->animationVec[i].duration, sizeFloat);
		cursor += sizeFloat;

		memcpy(cursor, &animation->animationVec[i].ticksPerSecond, sizeFloat);
		cursor += sizeFloat;

		memcpy(cursor, &animation->animationVec[i].channelsAmount, sizeInt);
		cursor += sizeInt;

		memcpy(cursor, &animation->animationVec[i].playing, sizeBool);
		cursor += sizeBool;

		for (int j = 0; j < animation->animationVec[i].channels.size(); j++) {

			varSize = sizeof(char) * animation->animationVec[i].channels[j].name.size() + 2;
			memcpy(cursor, &varSize, sizeInt);
			cursor += sizeInt;

			memcpy(cursor, (animation->animationVec[i].channels[j].name + "\0").c_str(), varSize);
			cursor += varSize;

			varSize = animation->animationVec[i].channels[j].positionKeys.size();
			memcpy(cursor, &varSize, sizeInt);
			cursor += sizeInt;

			for (std::map<float, float3>::iterator it = animation->animationVec[i].channels[j].positionKeys.begin(); it != animation->animationVec[i].channels[j].positionKeys.end(); it++) {

				memcpy(cursor, &it->first, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.x, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.y, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.z, sizeFloat);
				cursor += sizeFloat;

			}

			varSize = animation->animationVec[i].channels[j].rotationKeys.size();
			memcpy(cursor, &varSize, sizeInt);
			cursor += sizeInt;

			for (std::map<float, Quat>::iterator it = animation->animationVec[i].channels[j].rotationKeys.begin(); it != animation->animationVec[i].channels[j].rotationKeys.end(); it++) {

				memcpy(cursor, &it->first, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.x, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.y, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.z, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.w, sizeFloat);
				cursor += sizeFloat;

			}

			varSize = animation->animationVec[i].channels[j].scaleKeys.size();
			memcpy(cursor, &varSize, sizeInt);
			cursor += sizeInt;

			for (std::map<float, float3>::iterator it = animation->animationVec[i].channels[j].scaleKeys.begin(); it != animation->animationVec[i].channels[j].scaleKeys.end(); it++) {

				memcpy(cursor, &it->first, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.x, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.y, sizeFloat);
				cursor += sizeFloat;

				memcpy(cursor, &it->second.z, sizeFloat);
				cursor += sizeFloat;

			}

		}

	}

	App->externalManager->Save(path.c_str(), buffer, bufferSize);

	RELEASE_ARRAY(buffer);

}

void DataSaving::SaveAssetsMap(Application* App, std::map<std::string, FileInfo>* assetsMap) {

	char* buffer = nullptr;
	uint bufferSize;
	int index = 0;

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* assetsMapArray = (JsonManager::OpenArray(node, JSON_NODE_MAP_DATA));

	for (std::map<std::string, FileInfo>::iterator it = assetsMap->begin(); it != assetsMap->end(); it++) {

		json_array_append_value(assetsMapArray, json_value_init_object());
		JSON_Object* nodeComponent = json_array_get_object(assetsMapArray, index);

		json_object_set_string(nodeComponent, JSON_NODE_MAP_FILENAME, it->first.c_str());
		json_object_set_string(nodeComponent, JSON_NODE_MAP_FILEPATH, it->second.filePath.c_str());
		json_object_set_number(nodeComponent, JSON_NODE_MAP_ID, it->second.id);
		json_object_set_number(nodeComponent, JSON_NODE_MAP_LAST_CHANGE, it->second.lastTimeChanged);

		index++;

	}

	buffer = new char[JsonManager::GetArraySize(assetsMapArray)];
	uint size = JsonManager::Serialize(root.value, &buffer);

	std::string mapName = LIBRARY_PATH + (std::string)ASSETS_MAP + EXTENSION_MAP;
	App->externalManager->Save(mapName.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}


void DataSaving::SaveLibraryMap(Application* App, std::map<std::string, LibraryInfo>* libraryMap) {

	char* buffer = nullptr;
	uint bufferSize;
	int index = 0;

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* libraryMapArray(JsonManager::OpenArray(node, JSON_NODE_MAP_DATA));


	for (std::map<std::string, LibraryInfo>::iterator it = libraryMap->begin(); it != libraryMap->end(); it++) {

		json_array_append_value(libraryMapArray, json_value_init_object());
		JSON_Object* nodeComponent = json_array_get_object(libraryMapArray, index);

		json_object_set_string(nodeComponent, JSON_NODE_MAP_FILENAME, it->first.c_str());
		json_object_set_string(nodeComponent, JSON_NODE_MAP_FILEPATH, it->second.filePath.c_str());
		json_object_set_number(nodeComponent, JSON_NODE_MAP_TYPE, (int)it->second.type);

		index++;

	}

	buffer = new char[JsonManager::GetArraySize(libraryMapArray)];
	uint size = JsonManager::Serialize(root.value, &buffer);

	std::string mapName = LIBRARY_PATH + (std::string)LIBRARY_MAP + EXTENSION_MAP;
	App->externalManager->Save(mapName.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}


void DataSaving::SaveComponentJSON(Application* App, JSON_Array* gameComponentsArray, Component* component, int arrayIndex) {

	json_array_append_value(gameComponentsArray, json_value_init_object());
	JSON_Object* nodeComponent = json_array_get_object(gameComponentsArray, arrayIndex);
	json_object_set_number(nodeComponent, JSON_NODE_COMPONENT_TYPE, (int)component->type);
	json_object_set_number(nodeComponent, JSON_NODE_COMPONENT_ID, component->id);

	switch (component->type) {

	case COMPONENT_TYPE::MESH:

		SaveMesh(App, (Mesh*)component);

		break;

	case COMPONENT_TYPE::MATERIAL:

		SaveMaterial(App, (Material*)component);

		break;

	case COMPONENT_TYPE::ANIMATION:

		SaveAnimation(App, (Animation*)component);

		break;

	default:

		break;

	}

}





