#include "Material.h"

Material::Material(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MATERIAL), diffuseId(0), textureName() {}


Material::~Material() {}


void Material::SetDiffuse(uint id) { diffuseId = id; }



