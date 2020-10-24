#include "GameObject.h"

GameObject::GameObject(int _id, std::string _name, GameObject* _parent, bool _enabled) : name(_name), id(_id), parent(_parent), childs(), enabled(_enabled),
components(), componentIdGenerator(0), deleteGameObject(false) {
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

	for (int i = childs.size(); i > -1; i--) { childs[i]->PostUpdate(); }

	CheckGameObjectDeletion();
	CheckComponentDeletion();

}


Component* GameObject::AddComponent(COMPONENT_TYPE _type) {

	Component* component = nullptr;

	switch (_type) {

	case COMPONENT_TYPE::TRANSFORM:

		component = new Transform(GenerateId(), this);

		break;

	default:
		assert(true == false);
		break;
	}

	components.push_back(component);

	return component;
}


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


int GameObject::GenerateId() { return componentIdGenerator++; }


void GameObject::CheckComponentDeletion() { for (int i = components.size(); i > -1; i--) { if (components[i]->deleteComponent) { components.erase(components.begin() + i); } } }


void GameObject::CheckGameObjectDeletion() {

	for (int i = childs.size() - 1; i > -1; i--)
	{
		if (childs[i]->deleteGameObject) {
			delete childs[i];
			childs[i] = nullptr;
		}
	}

}


