#include "GameObject.h"
#include "OpenGLFuncionality.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

GameObject::GameObject(long long int _id, std::string _name, GameObject* _parent, bool _enabled, std::vector<GameObject*> children) :
	name(_name), id(_id), worldTransform(), parent(_parent), childs(children), enabled(_enabled), deleteGameObject(false), idGenerator(),
	mesh(nullptr), material(nullptr), transform(nullptr), camera(nullptr) {

	AddComponent(COMPONENT_TYPE::TRANSFORM, idGenerator.Int());
}

GameObject::~GameObject() {

	delete mesh;
	delete material;
	delete transform;
	delete camera;

	mesh = nullptr;
	material = nullptr;
	transform = nullptr;
	camera = nullptr;

	parent = nullptr;

}


void GameObject::Update() {

	int size = childs.size();
	for (int i = 0; i < size; i++) { if (childs[i]->enabled) { childs[i]->Update(); } }

}


void GameObject::PostUpdate(uint& defaultTextureId) {

	// TODO: try implementing dirty flag ;)

	if (parent != nullptr) { transform->RecalculateTransformFromParent(parent->worldTransform); }
	else { worldTransform = transform->transform; }
	if (camera != nullptr) { camera->UpdateTransform(); }

	for (int i = childs.size() - 1; i > -1; i--) { childs[i]->PostUpdate(defaultTextureId); }

	if (mesh != nullptr) {

		ManageAABB(mesh);

		if (material != nullptr) {

			if (material->diffuseId == 0) { OpenGLFunctionality::DrawMeshes(*mesh, worldTransform, defaultTextureId); }
			else { OpenGLFunctionality::DrawMeshes(*mesh, worldTransform, material->diffuseId); }

		}

		else { OpenGLFunctionality::DrawMeshes(*mesh, worldTransform, 0); }

		if (mesh->paintNormals) { OpenGLFunctionality::DrawLines(worldTransform, mesh->DebugNormals(), mesh->debugNormals); }

	}

	CheckComponentDeletion();

}


Component* GameObject::AddComponent(COMPONENT_TYPE _type, long long int id) {

	if (id == -1) { id = idGenerator.Int(); }

	switch (_type) {

	case COMPONENT_TYPE::TRANSFORM: if (transform == nullptr) { return transform = new Transform(id, this); }
	case COMPONENT_TYPE::MESH: if (mesh == nullptr) { return mesh = new Mesh(id, this); }
	case COMPONENT_TYPE::MATERIAL: if (material == nullptr) { return material = new Material(id, this); }
	case COMPONENT_TYPE::CAMERA: if (camera == nullptr) { return camera = new Camera(id, this); }
	default:
		assert(true == false);
		break;
	}

	return nullptr;

}


bool GameObject::AddGameObjectByParent(GameObject* newObject) {
	bool ret = false;

	if (newObject->parent->id == this->id) {
		childs.push_back(newObject);
		return true;
	}
	else {
		int size = childs.size();
		for (int i = 0; i < size; i++) {
			ret = childs[i]->AddGameObjectByParent(newObject);
			if (ret) { return ret; }
		}
	}

	return false;
}


bool GameObject::CheckChildDeletionById(long long int _id) {

	bool ret = false;

	if (id == _id) {
		SetDeleteGameObject();
		return true;
	}
	else {
		int size = childs.size();
		for (int i = 0; i < size; i++) {
			ret = childs[i]->CheckChildDeletionById(_id);
			if (ret) { return ret; }
		}
	}

	return false;
}


void GameObject::SetDeleteGameObject(bool deleteBool) {

	deleteGameObject = deleteBool;

	int size = childs.size();
	for (int i = 0; i < size; i++) { childs[i]->SetDeleteGameObject(deleteBool); }

}


void GameObject::SetDeleteComponent(COMPONENT_TYPE _type) {

	Component* component = GetComponent(_type);
	component->deleteComponent = true;

}


void GameObject::CheckComponentDeletion() {

	if (mesh != nullptr) { if (mesh->deleteComponent) { delete mesh; } }
	if (material != nullptr) { if (material->deleteComponent) { delete material; } }
	if (camera != nullptr) { if (camera->deleteComponent) { delete camera; } }

}


Component* GameObject::GetComponent(COMPONENT_TYPE _type) {

	switch (_type) {

	case COMPONENT_TYPE::TRANSFORM: return transform;
	case COMPONENT_TYPE::MESH: return mesh;
	case COMPONENT_TYPE::MATERIAL: return material;
	case COMPONENT_TYPE::CAMERA: return camera;
	default:
		assert(true == false);
		break;
	}

	return nullptr;

}


Component* GameObject::FindGameObjectChildByComponent(long long int componentId) {

	Component* ret = nullptr;

	if (transform != nullptr) { if (transform->id == componentId) { return transform; } }
	if (mesh != nullptr) { if (mesh->id == componentId) { return mesh; } }
	if (material != nullptr) { if (material->id == componentId) { return material; } }
	if (camera != nullptr) { if (camera->id == componentId) { return camera; } }

	for (int i = 0; i < childs.size(); i++) {

		ret = childs[i]->FindGameObjectChildByComponent(componentId);
		if (ret) { return ret; }

	}

	return ret;

}


GameObject* GameObject::FindGameObjectChild(long long int id) {

	GameObject* ret = nullptr;

	for (int i = 0; i < childs.size(); i++) {

		if (childs[i]->id == id) { return childs[i]; }
		else {
			ret = childs[i]->FindGameObjectChild(id);
			if (ret) { return ret; }
		}
	}

	return ret;

}


void GameObject::ManageAABB(Mesh* mesh) {

	OBB obb = mesh->boundingBox;
	obb.Transform(worldTransform);

	AABB newBoundingBox;
	newBoundingBox.SetNegativeInfinity();
	newBoundingBox.Enclose(obb);
	std::vector<float> cornerVec;

	for (int i = 0; i < 8; i++) {

		float3 corner = newBoundingBox.CornerPoint(i);
		cornerVec.push_back(corner.x);
		cornerVec.push_back(corner.y);
		cornerVec.push_back(corner.z);

	}

	OpenGLFunctionality::DrawBox(cornerVec);

}