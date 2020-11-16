#ifndef __JSON_MANAGER__
#define __JSON_MANAGER__

#include "parson/parson.h"

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;	// TODO: Can I kaboom this?


namespace JsonManager {

	int Serialize(JSON_Value* value, char** buffer);
	JSON_Array* OpenArray(JSON_Object* object, const char* name);
	JSON_Object* AddArrayNode(JSON_Array* jsonArray);
	int GetArraySize(JSON_Array* jsonArray);

};


#endif
