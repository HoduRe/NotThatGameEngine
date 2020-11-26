#include "Component.h"

Component::Component(long long int _id, GameObject* _gameObject, COMPONENT_TYPE _type, bool _enabled) : Resource(_id, ConvertComponentTypeToResourceType(_type)),
	type(_type), enabled(_enabled), owner(_gameObject), id(_id), deleteComponent(false) {}


Component::~Component() { owner = nullptr; }


void Component::Enable() { enabled = true; }


void Component::Disable() { enabled = false; }


void Component::SetComponentDelete() { deleteComponent = true; }


ResourceEnum Component::ConvertComponentTypeToResourceType(COMPONENT_TYPE type) {

	switch (type) {

	case COMPONENT_TYPE::CAMERA:

		return ResourceEnum::CAMERA;

	case COMPONENT_TYPE::MESH:

		return ResourceEnum::MESH;

	case COMPONENT_TYPE::MATERIAL:

		return ResourceEnum::MATERIAL;

	default:

		return ResourceEnum::UNKNOWN;

		break;
	}

	return ResourceEnum::UNKNOWN;

}


