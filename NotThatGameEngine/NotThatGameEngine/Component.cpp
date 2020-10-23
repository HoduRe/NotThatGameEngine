#include "Component.h"


Component::Component() : type(COMPONENT_TYPE::NONE), enabled(true) {}


Component::Component(COMPONENT_TYPE _type, bool _enabled) : type(_type), enabled(_enabled) {}


Component::~Component() {}


void Component::Enable() { enabled = true; }


void Component::Disable() { enabled = false; }


void Component::Update() {}


