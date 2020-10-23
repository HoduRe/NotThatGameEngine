#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

enum class COMPONENT_TYPE {
	NONE,



	ERROR_TYPE
};

class Component {

public:

	Component();
	Component(COMPONENT_TYPE _type, bool _enabled);
	~Component();

	virtual void Enable();
	virtual void Disable();
	virtual void Update();

public:

	bool enabled;
	COMPONENT_TYPE type;

};

#endif