#include "Component.h"
#include "Module.h"
#include "Application.h"


Component::Component(int _id, GameObject* _gameObject, COMPONENT_TYPE _type, bool _enabled) : type(_type), enabled(_enabled), owner(_gameObject), id(_id), deleteComponent(false) {}


Component::~Component() { owner = nullptr; }


void Component::Enable() { enabled = true; }


void Component::Disable() { enabled = false; }


void Component::Update() {}


void Component::ComponentClosed() { deleteComponent = true; }



