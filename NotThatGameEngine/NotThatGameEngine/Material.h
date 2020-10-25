#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Component.h"
#include "Globals.h"

class GameObject;

class Material : public Component {

public:

	Material(int _id, GameObject* _gameObject);
	~Material();

	void Update();

public:

	std::vector<uint> textureIdVec;

};

#endif