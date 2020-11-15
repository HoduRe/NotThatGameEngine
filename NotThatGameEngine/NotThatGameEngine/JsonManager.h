#ifndef __JSON_MANAGER__
#define __JSON_MANAGER__

#include "parson/parson.h"

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;


class JsonManager {

public:

	JsonManager();	// Write file
	JsonManager(JSON_Object* object);	// Write an array
	JsonManager(const char* buffer);	// Read file
	~JsonManager();

	void SetInt(const char* name, int data);
	void SetFloat(const char* name, float data);
	void SetString(const char* name, const char* data);
	void SetBool(const char* name, bool data);
	void AddFloat(JSON_Array* arrayObject, const float& data);

	int Serialize(char** buffer);

	JSON_Array* OpenArray(const char* name);
	JsonManager AddArrayNode(JSON_Array* jsonArray);
	int GetArraySize(JSON_Array* jsonArray);

private:

	JSON_Value* root_value = nullptr;
	JSON_Object* node = nullptr;


};


#endif
