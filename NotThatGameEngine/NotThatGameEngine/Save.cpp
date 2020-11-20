#include "Save.h"
#include "Application.h"
#include "JsonManager.h"

void DataSaving::SaveScene(Application* App) {

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.value));
	JSON_Array* gameObjectsArray(JsonManager::OpenArray(node, JSON_NODE_GAMEOBJECTS));

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;

	for (int i = 0; i < gameObjects.size(); i++) {

		for (int j = 0; j < gameObjects[i]->childs.size(); j++) { gameObjects.push_back(gameObjects[i]->childs[j]); } // TODO: This should be called recursively :)

		SaveGameObject(App, JsonManager::AddArrayNode(gameObjectsArray), gameObjects[i]);

	}

	// TODO: Save cameras, lights...

	char* buffer = new char[JsonManager::GetArraySize(gameObjectsArray)];
	uint size = JsonManager::Serialize(root.value, &buffer);

	std::string sceneName = LIBRARY_PATH + (std::string)"Scene1" + EXTENSION_SCENES;
	App->externalManager->Save(sceneName.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);

}



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

	std::string path = (std::string)MATERIALS_PATH + std::to_string(material->id) + EXTENSION_MATERIALS;
	char* buffer = nullptr;
	uint bufferSize;
	std::string textureName;
	int nameSize;

	if (material->diffuseId != 0) {

		textureName = App->texture->GetTextureData(material->diffuseId)->name;
		nameSize = textureName.size();
		bufferSize = textureName.size() + sizeof(int);

		buffer = new char[bufferSize];
		char* cursor = buffer;

		memcpy(cursor, &nameSize, sizeof(int));
		cursor += sizeof(int);

		memcpy(cursor, textureName.c_str(), nameSize);

		App->externalManager->Save(path.c_str(), buffer, bufferSize);

	}

	else {
		
		nameSize = 0;
		buffer = new char[sizeof(int)];
		memcpy(buffer, &nameSize, sizeof(int));
		App->externalManager->Save(path.c_str(), buffer, 0);
	
	}

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

			SaveComponent(App, components[i]);

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





