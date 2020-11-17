#include "Application.h"
#include "JsonManager.h"
#include "GameObject.h"
#include "SaveLoad.h"


ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	LoadScene();

	return true;

}


bool ResourceManager::Start()
{

	App->eventManager->EventRegister(EVENT_ENUM::SAVE_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::LOAD_SCENE, this);

	return true;
}


update_status ResourceManager::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::PostUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


bool ResourceManager::CleanUp() {

	return true;

}


// TODO: Move LoadScene and SaveScene to SaveLoad.h, or the new place to be saving things (SaveLoadComponents, SaveLoadX...), because if we implement models, we will have to have an intermedium between gameobject and scene
void ResourceManager::LoadScene() {

	// TODO: Load scene
	std::vector<std::string> scenesVec;
	App->externalManager->GetAllFilesWithExtension(LIBRARY_PATH, "NotThatScene", scenesVec);	// TODO: we will have to load all paths, no matter how folders are called or organized

	if (scenesVec.size() != 0) {	// TODO: Should we load more than one scene? No, right?

		JsonManager::JsonValue root(json_parse_string(scenesVec[0].c_str()));
		JSON_Object* node(json_value_get_object(root.root));
		JSON_Array* gameObjectsArray(json_object_get_array(node, JSON_NODE_GAMEOBJECTS));

		int size = JsonManager::GetArraySize(gameObjectsArray);

		for (int i = 0; i < size; i++) {

			JSON_Object* itNode = json_array_get_object(gameObjectsArray, i);

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

			for (int i = 0; i < JsonManager::GetArraySize(componentNode); i++) {



			}

//#define JSON_NODE_COMPONENTS "Components"
//#define JSON_NODE_COMPONENT_TYPE "ComponentType"
//#define JSON_NODE_COMPONENT_ID "ComponentID"

			App->editorScene->AddGameObject(gameObject);

		}

	}
	// TODO: Load all gameobjects using JSON

	// TODO: Load all gameobjects' components

	// TODO: Add components to gameobjects

	// TODO: Load other components (camera)


	// TODO: Load other objects which are not "saved"

}


void ResourceManager::SaveScene() {

	JsonManager::JsonValue root(json_value_init_object());
	JSON_Object* node(json_value_get_object(root.root));
	JSON_Array* gameObjectsArray(JsonManager::OpenArray(node, JSON_NODE_GAMEOBJECTS));

	std::vector<GameObject*> gameObjects = App->editorScene->rootGameObjectsVec;

	for (int i = 0; i < gameObjects.size(); i++) {

		for (int j = 0; j < gameObjects[i]->childs.size(); j++) { gameObjects.push_back(gameObjects[i]->childs[j]); } // TODO: This should be called recursively :)

		DataSaving::SaveGameObject(App, JsonManager::AddArrayNode(gameObjectsArray), gameObjects[i]);

	}

	// TODO: Save cameras, lights...

	char* buffer = new char[JsonManager::GetArraySize(gameObjectsArray)];
	uint size = JsonManager::Serialize(root.root, &buffer);

	std::string sceneName = LIBRARY_PATH + (std::string)"Scene1.NotThatScene";
	App->externalManager->Save(sceneName.c_str(), buffer, size);

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		SaveScene();

		break;

	case EVENT_ENUM::LOAD_SCENE:

		LoadScene();

		break;

	default:
		break;
	}

	return true;
}




