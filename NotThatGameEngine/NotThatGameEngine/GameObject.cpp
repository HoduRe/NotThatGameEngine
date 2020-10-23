#include "GameObject.h"

GameObject::GameObject(int _id, std::string _name, GameObject* _parent, bool _enabled) : name(_name), id(_id), parent(_parent), childs(), enabled(_enabled), components(), componentIdGenerator(0) {
	AddComponent(COMPONENT_TYPE::TRANSFORM);
}

// TODO how do we deal with the parent-child conflict; How do we delete a gameobject without leaving it's child / parent node dirty			width = *static_cast<int*>(var) * SCREEN_SIZE;
GameObject::~GameObject() {

	int childsSize = childs.size();
	int componentsSize = components.size();

	for (int i = childsSize; i > -1; i--)
	{
		delete childs[i];
		childs[i] = nullptr;
	}

	for (int i = componentsSize - 1; i > -1; i--) {
		delete components[i];
		components[i] = nullptr;
	}

}


void GameObject::Update() {

	int size = components.size();
	for (int i = 0; i < size; i++) { if (components[i]->enabled) { components[i]->Update(); } }

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


int GameObject::GenerateId() { return componentIdGenerator++; }



