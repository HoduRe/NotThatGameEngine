#include "Camera.h"

Camera::Camera(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::CAMERA), camera() {}


Camera::~Camera() {}


void Camera::Update() {



}


void Camera::PostUpdate() {



}

