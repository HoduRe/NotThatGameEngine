#include "GameObject.h"
#include "OpenGLFuncionality.h"

GameObject::GameObject(int _id, std::string _name, GameObject* _parent, bool _enabled, std::vector<GameObject*> children) :
	name(_name), id(_id), worldTransform(), parent(_parent), childs(children), enabled(_enabled), components(), componentIdGenerator(0), deleteGameObject(false) {
	AddComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::~GameObject() {

	for (int i = childs.size() - 1; i > -1; i--)
	{
		delete childs[i];
		childs[i] = nullptr;
	}

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

	std::vector<Component*> meshVec = FindComponents(COMPONENT_TYPE::MESH);
	Material* material = (Material*)FindComponent(COMPONENT_TYPE::MATERIAL);
	Mesh* mesh = nullptr;

	for (int i = meshVec.size() - 1; i > -1; i--) {

		mesh = (Mesh*)meshVec[i];
		if (material != nullptr) {
			if (material->diffuseId == 0) { DrawMeshes(*mesh, worldTransform, defaultTextureId); }
			else { DrawMeshes(*mesh, worldTransform, material->diffuseId); }
		}
		else { DrawMeshes(*mesh, worldTransform, 0); }

	}

	CheckGameObjectDeletion();
	CheckComponentDeletion();

}


Component* GameObject::AddComponent(COMPONENT_TYPE _type) {

	Component* component = nullptr;

	switch (_type) {

	case COMPONENT_TYPE::TRANSFORM:

		component = FindComponent(COMPONENT_TYPE::TRANSFORM);
		if (component == nullptr) { component = new Transform(GenerateComponentId(), this); }
		else { return component; }

		break;

	case COMPONENT_TYPE::MESH:

		component = new Mesh(GenerateComponentId(), this);

		break;

	case COMPONENT_TYPE::MATERIAL:

		component = FindComponent(COMPONENT_TYPE::MATERIAL);
		if (component == nullptr) { component = new Material(GenerateComponentId(), this); }
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

	if (newObject->parent == this) {
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


int GameObject::GenerateComponentId() { return componentIdGenerator++; }


bool GameObject::CheckChildDeletionById(int _id) {

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


void GameObject::SetDeleteGameObject() {

	deleteGameObject = true;

	int size = childs.size();
	for (int i = 0; i < size; i++) { childs[i]->SetDeleteGameObject(); }

}


void GameObject::CheckComponentDeletion() { for (int i = components.size() - 1; i > -1; i--) { if (components[i]->deleteComponent) { components.erase(components.begin() + i); } } }


void GameObject::CheckGameObjectDeletion() {

	for (int i = childs.size() - 1; i > -1; i--)
	{
		if (childs[i]->deleteGameObject) {
			delete childs[i];
			childs[i] = nullptr;
		}
	}

}


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



