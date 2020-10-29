#include "Application.h"
#include "ObjectLoader.h"
#include "Primitives.h"

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


EditorScene::EditorScene(Application* app, bool start_enabled) : Module(app, start_enabled), gameObjectIdCount(0), rootGameObjectsVec(), stream(), defaultTextureId(0), focus(nullptr) {}


EditorScene::~EditorScene() {

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) {
		delete rootGameObjectsVec[i];
		rootGameObjectsVec[i] = nullptr;
	}

	focus = nullptr;

}


bool EditorScene::Init() {

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	App->eventManager->EventRegister(EVENT_ENUM::CREATE_CUBE, this);
	App->eventManager->EventRegister(EVENT_ENUM::CREATE_SPHERE, this);
	App->eventManager->EventRegister(EVENT_ENUM::CREATE_PYRAMID, this);
	App->eventManager->EventRegister(EVENT_ENUM::CREATE_CYLINDER, this);
	App->eventManager->EventRegister(EVENT_ENUM::DEFAULT_TEXTURE_LOADED, this);

	return true;

}


bool EditorScene::Start()
{

	LoadModel(App, "Library/Meshes/BakerHouse.fbx");

	return true;
}


update_status EditorScene::PreUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


update_status EditorScene::Update(float dt)
{

	int size = rootGameObjectsVec.size();
	for (int i = 0; i < size; i++) { rootGameObjectsVec[i]->Update(); }

	return update_status::UPDATE_CONTINUE;

}


update_status EditorScene::PostUpdate(float dt) {

	App->renderer3D->SetFrameBuffer(App->renderer3D->frameBufferId);

	int size = rootGameObjectsVec.size();
	for (int i = 0; i < size; i++) { rootGameObjectsVec[i]->PostUpdate(defaultTextureId); }

	DeleteRootGameObjects();

	App->renderer3D->SetFrameBuffer(0);

	return update_status::UPDATE_CONTINUE;

}


bool EditorScene::CleanUp() {

	aiDetachAllLogStreams();
	return true;

}


int EditorScene::GenerateId() { return gameObjectIdCount++; }


bool EditorScene::AddGameObject(GameObject* newObject) {

	bool ret = false;

	if (newObject != nullptr) {

		newObject->name = newObject->name + std::to_string(newObject->id);

		if (newObject->parent == nullptr) {
		
			rootGameObjectsVec.push_back(newObject);
			return true;

		}

		else {

			int size = rootGameObjectsVec.size();

			for (int i = 0; i < size; i++) {

				if (rootGameObjectsVec[i] == newObject->parent) { rootGameObjectsVec[i]->childs.push_back(newObject); }
				else {

					ret = rootGameObjectsVec[i]->AddGameObjectByParent(newObject);
					if (ret) { return ret; }
				}

			}
		}
	}

	LOG("GameObject couldn't be added into the scene.");

	return ret;

}


bool EditorScene::AddPrimitive(PrimitiveEnum _type) {

	GameObject* newObject = new GameObject(GenerateId(), "");
	Mesh* mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);

	switch (_type) {

	case PrimitiveEnum::PRIMITIVE_CUBE:

		newObject->name = "Cube";
		SetCubeVertices(3.0f, &mesh->vertices, &mesh->indices);

		break;

	case PrimitiveEnum::PRIMITIVE_SPHERE:

		newObject->name = "Sphere";
		SetSphereVertices(2.0f, 36, 18, &mesh->vertices, &mesh->indices);

		break;

	case PrimitiveEnum::PRIMITIVE_PYRAMID:

		newObject->name = "Pyramid";
		SetPyramidVertices(5.0f, 2.0f, &mesh->vertices, &mesh->indices);

		break;

	case PrimitiveEnum::PRIMITIVE_CYLINDER:

		newObject->name = "Cylinder";
		SetCylinderVertices(8.0f, 3.0f, 30, &mesh->vertices, &mesh->indices);

		break;

	default:

		break;
	}

	mesh->SetVertices(mesh->vertices);
	mesh->SetIndices(mesh->indices);

	AddGameObject(newObject);

	return true;
}


void EditorScene::DeleteGameObject(int id) {

	bool ret = false;

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) {
		if (rootGameObjectsVec[i]->id == id) { rootGameObjectsVec[i]->SetDeleteGameObject(); }
		else {
			ret = rootGameObjectsVec[i]->CheckChildDeletionById(id);
			if (ret) { return; }
		}
	}

}


void EditorScene::DeleteRootGameObjects() {

	for (int i = rootGameObjectsVec.size() - 1; i > -1; i--) { if (rootGameObjectsVec[i]->deleteGameObject) { rootGameObjectsVec.erase(rootGameObjectsVec.begin() + i); } }

}


bool EditorScene::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	switch (eventId) {

	case EVENT_ENUM::DEFAULT_TEXTURE_LOADED:

		defaultTextureId = App->texture->defaultTextureId;
		break;

	case EVENT_ENUM::CREATE_CUBE:

		AddPrimitive(PrimitiveEnum::PRIMITIVE_CUBE);

		break;

	case EVENT_ENUM::CREATE_SPHERE:

		AddPrimitive(PrimitiveEnum::PRIMITIVE_SPHERE);

		break;

	case EVENT_ENUM::CREATE_PYRAMID:

		AddPrimitive(PrimitiveEnum::PRIMITIVE_PYRAMID);

		break;

	case EVENT_ENUM::CREATE_CYLINDER:

		AddPrimitive(PrimitiveEnum::PRIMITIVE_CYLINDER);

		break;

	default:
		break;
	}

	return false;
}


void EditorScene::SetFocus(GameObject* gameobject) { focus = gameobject; }


GameObject* EditorScene::GetFocus() { return focus; }



