#include "GameObject.h"
#include "OpenGLFuncionality.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

GameObject::GameObject(long long int _id, std::string _name, GameObject* _parent, bool _enabled, std::vector<GameObject*> children) :
	name(_name), id(_id), worldTransform(), parent(_parent), childs(children), enabled(_enabled), components(), deleteGameObject(false), idGenerator() {

	AddComponent(COMPONENT_TYPE::TRANSFORM);

}

GameObject::~GameObject() {

	for (int i = components.size() - 1; i > -1; i--) {
		delete components[i];
		components[i] = nullptr;
	}

	parent = nullptr;

}


void GameObject::Update() {

	int size = childs.size();
	for (int i = 0; i < size; i++) { if (childs[i]->enabled) { childs[i]->Update(); } }

	size = components.size();
	for (int i = 0; i < size; i++) { if (components[i]->enabled) { components[i]->Update(); } }

}


void GameObject::PostUpdate(uint& defaultTextureId) {

	// TODO: try implementing dirty flag ;)
	Transform* transform = (Transform*)FindComponent(COMPONENT_TYPE::TRANSFORM);

	if (parent != nullptr) { transform->RecalculateTransformFromParent(parent->worldTransform); }
	else { worldTransform = transform->transform; }

	for (int i = childs.size() - 1; i > -1; i--) { childs[i]->PostUpdate(defaultTextureId); }

	Mesh* mesh = (Mesh*)FindComponent(COMPONENT_TYPE::MESH);
	Material* material = (Material*)FindComponent(COMPONENT_TYPE::MATERIAL);

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


Component* GameObject::AddComponent(COMPONENT_TYPE _type, long long int _id) {

	Component* component = nullptr;
	long long int id = _id;

	if (_id == 0) { id = GenerateComponentId(); }

	switch (_type) {

	case COMPONENT_TYPE::TRANSFORM:

		component = FindComponent(COMPONENT_TYPE::TRANSFORM);
		if (component == nullptr) { component = new Transform(id, this); }
		else { return component; }

		break;

	case COMPONENT_TYPE::MESH:

		component = FindComponent(COMPONENT_TYPE::MESH);
		if (component == nullptr) { component = new Mesh(id, this); }
		else { return component; }

		break;

	case COMPONENT_TYPE::MATERIAL:

		component = FindComponent(COMPONENT_TYPE::MATERIAL);
		if (component == nullptr) { component = new Material(id, this); }
		else { return component; }

		break;

	case COMPONENT_TYPE::CAMERA:

		component = FindComponent(COMPONENT_TYPE::CAMERA);
		if (component == nullptr) { component = new Camera(id, this); }
		else { return component; }

		break;

	default:
		assert(true == false);
		break;
	}

	components.push_back(component);

	return component;
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


long long int GameObject::GenerateComponentId() { return idGenerator.Int(); }


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

	Component* component = FindComponent(_type);
	component->deleteComponent = true;

}


void GameObject::CheckComponentDeletion() { for (int i = components.size() - 1; i > -1; i--) { if (components[i]->deleteComponent) { components.erase(components.begin() + i); } } }


Component* GameObject::FindComponent(COMPONENT_TYPE _type) {

	int size = components.size();
	for (int i = 0; i < size; i++) { if (components[i]->type == _type) { return components[i]; } }
	return nullptr;

}


std::vector<Component*> GameObject::FindComponents(COMPONENT_TYPE _type) {

	std::vector<Component*> vec;
	int size = components.size();
	for (int i = 0; i < size; i++) { if (components[i]->type == _type) { vec.push_back(components[i]); } }
	return vec;

}


Component* GameObject::FindGameObjectChildByComponent(long long int componentId) {

	Component* ret = nullptr;

	for (int i = 0; i < components.size(); i++) { if (components[i]->id == componentId) { return components[i]; } }

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