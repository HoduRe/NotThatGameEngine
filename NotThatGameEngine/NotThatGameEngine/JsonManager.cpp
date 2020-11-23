#include "JsonManager.h"
#include "parson/parson.h"

JsonManager::JsonValue::JsonValue() : value() {}


JsonManager::JsonValue::JsonValue(JSON_Value* _value) : value(_value) {}


JsonManager::JsonValue::~JsonValue() { if (value) { json_value_free(value); } }


/*JsonManager::JsonObject::JsonObject() : object() {}


JsonManager::JsonObject::JsonObject(JSON_Object* _object) : object(_object) {}	// TODO: Jose said that reusing nodes (JSON_Objects) causes a memory leak, but that was on Marc code. This is a better code XD Anyway, check if it still happens


JsonManager::JsonObject::~JsonObject() { if (object) { json_object_clear(object); } }


JsonManager::JsonArray::JsonArray() : jsonArray() {}


JsonManager::JsonArray::JsonArray(JSON_Array* _array) : jsonArray(_array) {}


JsonManager::JsonArray::~JsonArray() { if (jsonArray) { json_array_clear(jsonArray); } } // TODO: This makes scale not safe, because it's the last node, and when JSON_Array gets deleted goes kaboom	*/


JSON_Array* JsonManager::OpenArray(JSON_Object* object, const char* name) {

	json_object_set_value(object, name, json_value_init_array());
	return json_object_get_array(object, name);

}


JSON_Object* JsonManager::AddArrayNode(JSON_Array* jsonArray) {

	json_array_append_value(jsonArray, json_value_init_object());
	return json_array_get_object(jsonArray, GetArraySize(jsonArray) - 1);

}


int JsonManager::Serialize(JSON_Value* value, char** buffer) {

	size_t size = json_serialization_size_pretty(value);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(value, *buffer, size);
	return size;

}


int JsonManager::GetArraySize(JSON_Array* jsonArray) { return json_array_get_count(jsonArray); }



