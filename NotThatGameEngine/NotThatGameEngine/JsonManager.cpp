#include "JsonManager.h"
#include "Globals.h"

JsonManager::JsonManager() {

	root_value = json_value_init_object();
	node = json_value_get_object(root_value);

}


JsonManager::JsonManager(const char* buffer) {

	root_value = json_parse_string(buffer);
	if (root_value) { node = json_value_get_object(root_value); }

}


JsonManager::JsonManager(JSON_Object* object) : node(object) {}


JsonManager::~JsonManager() { if (root_value) { json_value_free(root_value); } }


void JsonManager::SetNumber(const char* name, int data) { json_object_set_number(node, name, data); }


void JsonManager::SetString(const char* name, const char* data) { json_object_set_string(node, name, data); }


void JsonManager::SetBool(const char* name, bool data) { json_object_set_boolean(node, name, data); }


JSON_Array* JsonManager::OpenArray(const char* name) {

	json_object_set_value(node, name, json_value_init_array());
	return json_object_get_array(node, name);

}


JsonManager JsonManager::AddArrayNode(JSON_Array* jsonArray) {

	uint size = json_array_get_count(jsonArray);
	json_array_append_value(jsonArray, json_value_init_object());
	size++;
	return JsonManager(json_array_get_object(jsonArray, size - 1));

}


int JsonManager::Serialize(char** buffer) {

	size_t size = json_serialization_size_pretty(root_value);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(root_value, *buffer, size);
	return size;

}


