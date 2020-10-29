#include "Material.h"

Material::Material(int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MATERIAL), diffuseId(0) {}


Material::~Material() {}


void Material::Update() {}


void Material::SetDiffuse(uint id) { diffuseId = id; }



