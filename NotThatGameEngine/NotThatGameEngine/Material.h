#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Component.h"
#include "Globals.h"

class GameObject;
class Application;

class Material : public Component {

public:

	Material(long long int _id, GameObject* _gameObject);
	~Material();

	std::string GetTextureName();
	void SetTextureName(Application* App, std::string name);

public:


private:

	std::string textureName;

};

#endif