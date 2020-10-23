#include "Component.h"


Component::Component(int _id, GameObject* _gameObject, COMPONENT_TYPE _type, bool _enabled) : type(_type), enabled(_enabled), owner(_gameObject), id(_id) {}


Component::~Component() { owner = nullptr; }


void Component::Enable() { enabled = true; }


void Component::Disable() { enabled = false; }


void Component::Update() {}


