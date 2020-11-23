#include "Application.h"
#include "EditorScene.h"
#include "ManagerEvent.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "ModelImporter.h"
#include "Textures.h"
#include "Primitives.h"
#include "Save.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"

#pragma comment( lib, "Assimp/libx86/assimp.lib" )


EditorScene::EditorScene(Application* app, bool start_enabled) : Module(app, start_enabled),
rootGameObjectsVec(), stream(), defaultTextureId(0), focus(nullptr), sceneWindowFocus(false), idGenerator() {}


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
	App->eventManager->EventRegister(EVENT_ENUM::PUT_TEXTURE_TO_FOCUSED_MODEL, this);

	return true;

}


bool EditorScene::Start()
{

	App->camera->Move(vec3(0.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	ModelImporter::LoadNewModel(App, "Meshes/BakerHouse.fbx");

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


bool EditorScene::AddGameObject(GameObject* newObject) {	// This function doesn't work if you haven't added the parent previously!

	bool ret = false;

	if (newObject != nullptr) {

		if (newObject->parent == nullptr) {

			rootGameObjectsVec.push_back(newObject);
			LOG("Object %s added into scene.\n", newObject->name.c_str());
			return true;

		}

		else {

			int size = rootGameObjectsVec.size();

			for (int i = 0; i < size; i++) {

				if (rootGameObjectsVec[i] == newObject->parent) {

					rootGameObjectsVec[i]->childs.push_back(newObject);
					LOG("Object %s added into scene.\n", newObject->name.c_str());
					return true;

				}

				else {

					ret = rootGameObjectsVec[i]->AddGameObjectByParent(newObject);
					if (ret) {

						LOG("Object %s added into scene.\n", newObject->name.c_str());
						return ret;

					}

				}

			}
		}
	}

	LOG("The object couldn't be added into the scene.");

	return ret;

}


bool EditorScene::AddPrimitive(PrimitiveEnum _type) {

	GameObject* newObject = nullptr;
	Mesh* mesh = nullptr;
	long long int id = GenerateId();

	switch (_type) {

	case PrimitiveEnum::PRIMITIVE_CUBE:

		newObject = new GameObject(id, "Cube");
		mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);
		PrimitivesF::SetCubeVertices(3.0f, &mesh->vertices, &mesh->indices);

		break;

	case PrimitiveEnum::PRIMITIVE_SPHERE:

		newObject = new GameObject(id, "Sphere");
		mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);
		PrimitivesF::SetSphereVertices(2.0f, 36, 18, &mesh->vertices, &mesh->indices);

		break;

	case PrimitiveEnum::PRIMITIVE_PYRAMID:

		newObject = new GameObject(id, "Pyramid");
		mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);
		PrimitivesF::SetPyramidVertices(5.0f, 2.0f, &mesh->vertices, &mesh->indices);

		break;

	case PrimitiveEnum::PRIMITIVE_CYLINDER:

		newObject = new GameObject(id, "Cylinder");
		mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);
		PrimitivesF::SetCylinderVertices(8.0f, 3.0f, 30, &mesh->vertices, &mesh->indices);

		break;

	default:

		break;
	}

	mesh->SetVertices(mesh->vertices);
	mesh->SetIndices(mesh->indices);

	DataSaving::SaveMesh(App, mesh);

	AddGameObject(newObject);

	return true;
}


GameObject* EditorScene::FindGameObject(long long int id) {

	GameObject* ret = nullptr;

	for (int i = 0; i < rootGameObjectsVec.size(); i++) {

		if (rootGameObjectsVec[i]->id == id) { return rootGameObjectsVec[i]; }
		else {
			ret = rootGameObjectsVec[i]->FindGameObjectChild(id);
			if (ret) { return ret; }
		}
	}

	return ret;

}


void EditorScene::DeleteGameObject(long long int id) {

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


Component* EditorScene::FindGameObjectByComponent(long long int componentId) {

	Component* ret = nullptr;

	for (int i = 0; i < rootGameObjectsVec.size(); i++) {

		for (int j = 0; j < rootGameObjectsVec[i]->components.size(); j++) {

			if (rootGameObjectsVec[i]->components[j]->id == componentId) { return rootGameObjectsVec[i]->components[j]; }

		}

		ret = rootGameObjectsVec[i]->FindGameObjectChildByComponent(componentId);
		if (ret) { return ret; }

	}

	return ret;

}

bool EditorScene::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	Material* material;

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

	case EVENT_ENUM::PUT_TEXTURE_TO_FOCUSED_MODEL:

		if (focus != nullptr) {

			material = (Material*)focus->FindComponent(COMPONENT_TYPE::MATERIAL);

			if (material != nullptr) { material->diffuseId = (uint)var; }
			else { LOG("This object has no material. Assign it a material through the inspector.\n"); }

		}

		else { LOG("There is no object selected to drop the texture to.\n"); }

		break;

	default:
		break;
	}

	return true;
}


void EditorScene::SetFocus(GameObject* gameobject) { focus = gameobject; }


GameObject* EditorScene::GetFocus() { return focus; }


long long int EditorScene::GenerateId() { return idGenerator.Int(); }

