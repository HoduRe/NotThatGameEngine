#include "Transform.h"


Transform::Transform(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::TRANSFORM) {}


Transform::~Transform() {}



