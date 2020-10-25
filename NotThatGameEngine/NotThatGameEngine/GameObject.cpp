#include "GameObject.h"
#include "OpenGLFuncionality.h"

GameObject::GameObject(int _id, std::string _name, GameObject* _parent, bool _enabled, std::vector<GameObject*> children) :
	name(_name), id(_id), parent(_parent), childs(children), enabled(_enabled), components(), componentIdGenerator(0), deleteGameObject(false) {
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


void GameObject::PostUpdate() {

	for (int i = childs.size() -1; i > -1; i--) { childs[i]->PostUpdate(); }

	Mesh* mesh = nullptr;
	int subMeshSize;
	int size = components.size();

	if (size != 0) {
		for (int i = 0; i < size; i++) {
			if (components[i]->type == COMPONENT_TYPE::MESH) {
				mesh = (Mesh*)components[i];
				subMeshSize = mesh->subMeshes.size();
				for (int j = 0; j < subMeshSize; j++) { DrawMeshes(mesh->subMeshes[j]); }
			}
		}
	}

	CheckGameObjectDeletion();
	CheckComponentDeletion();

}


Component* GameObject::AddComponent(COMPONENT_TYPE _type) {

	Component* component = nullptr;

	switch (_type) {

	case COMPONENT_TYPE::TRANSFORM:

		component = new Transform(GenerateComponentId(), this);

		break;

	case COMPONENT_TYPE::MESH:

		component = new Mesh(GenerateComponentId(), this);

		break;

	case COMPONENT_TYPE::MATERIAL:

		component = new Material(GenerateComponentId(), this);

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


void GameObject::CheckComponentDeletion() { for (int i = components.size()- 1; i > -1; i--) { if (components[i]->deleteComponent) { components.erase(components.begin() + i); } } }


void GameObject::CheckGameObjectDeletion() {

	for (int i = childs.size() - 1; i > -1; i--)
	{
		if (childs[i]->deleteGameObject) {
			delete childs[i];
			childs[i] = nullptr;
		}
	}

}


