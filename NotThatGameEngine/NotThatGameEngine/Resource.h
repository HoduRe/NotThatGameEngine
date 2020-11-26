#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "ManagerResource.h"

class Resource {

public:

	Resource(long long int _id, ResourceEnum _type);
	~Resource();

public:

	long long int id;
	ResourceEnum resourceType;

};

#endif
