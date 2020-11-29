#include "Material.h"
#include "Application.h"
#include "Textures.h"
#include "ManagerEvent.h"
#include "GameObject.h"

Material::Material(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::MATERIAL), textureName() {}


Material::~Material() { if (textureName.empty() == false) { owner->App->texture->DecreaseTextureCount(textureName); } }


std::string Material::GetTextureName() { return textureName; }


void Material::SetTextureName(Application* App, std::string name) {	// This should probably be controlle by resource manager, but I think I fucked myself by how I've done things. And not going to redo resource manager for a third time

	if (textureName.empty() == false) { App->texture->DecreaseTextureCount(textureName); }
	if (name.empty() == false) {
		textureName = name;
		if (App->texture->IsTextureRepeated(textureName) == 0) { App->eventManager->GenerateEvent(EVENT_ENUM::FILE_LOADING, EVENT_ENUM::NULL_EVENT, (char*)textureName.c_str()); }
		else { App->texture->IncreaseTextureCount(textureName); }
	}
}


