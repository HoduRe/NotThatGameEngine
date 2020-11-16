#include "JsonManager.h"

JSON_Array* JsonManager::OpenArray(JSON_Object* object, const char* name) {

	json_object_set_value(object, name, json_value_init_array());
	return json_object_get_array(object, name);

}


JSON_Object* JsonManager::AddArrayNode(JSON_Array* jsonArray) {

	json_array_append_value(jsonArray, json_value_init_object());
	return json_array_get_object(jsonArray, GetArraySize(jsonArray)-1);

}


int JsonManager::Serialize(JSON_Value* value, char** buffer) {

	size_t size = json_serialization_size_pretty(value);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(value, *buffer, size);
	return size;

}


int JsonManager::GetArraySize(JSON_Array* jsonArray) { return json_array_get_count(jsonArray); }



