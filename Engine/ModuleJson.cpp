#include "ModuleJson.h"
#include "parson/parson.h"
#include "MathGeoLib/include/MathGeoLib.h"

//JsonObject ====================================================

JsonObj::JsonObj()
{
	root = json_value_init_object();
	object = json_value_get_object(root);
}

JsonObj::JsonObj(const char* buffer) : object(nullptr)
{
	root = json_parse_string(buffer);

	if (root != NULL)
	{
		object = json_value_get_object(root);
		LOG("File parsed successfully");
	}
	else
	{
		LOG("Error trying to load config file");
	}
}

JsonObj::JsonObj(JSON_Object* object) : root(nullptr)
{
	object = object;
}

JsonObj::~JsonObj()
{
	object = nullptr;

	if (root != nullptr)
	{
		//json_value_free(_root);
		root = nullptr;
	}
}

void JsonObj::Release()
{
	json_value_free(root);
}

JSON_Object* JsonObj::GetJSONObject()
{
	return object;
}

JSON_Value* JsonObj::GetValue()
{
	return root;
}

uint JsonObj::Save(char** buffer)
{
	uint size = json_serialization_size_pretty(root);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(root, *buffer, size);
	return size;
}

JSON_Array* JsonObj::GetArray(const char* name)
{
	if (json_object_has_value(object, name) == 1)
		return json_object_get_array(object, name);
	else
		return nullptr;
}

int JsonObj::GetInt(const char* name, int default)
{
	if (json_object_has_value(object, name) == 1)
		return json_object_get_number(object, name);
	else
		return default;
}

float JsonObj::GetFloat(const char* name, float default)
{
	if (json_object_has_value(object, name) == 1)
		return json_object_get_number(object, name);
	else
		return default;
}

bool JsonObj::GetBool(const char* name, bool default)
{
	if (json_object_has_value(object, name) == 1)
		return json_object_get_boolean(object, name);
	else
		return default;
}

const char* JsonObj::GetString(const char* name, const char* default)
{
	if (json_object_has_value(object, name) == 1)
		return json_object_get_string(object, name);
	else
		return default;
}

float3 JsonObj::GetFloat3(const char* name, float3 default)
{
	if (json_object_has_value(object, name) == 1)
	{
		float3 number;

		JSON_Array* array;
		array = json_object_get_array(object, name);

		number.x = json_array_get_number(array, 0);
		number.y = json_array_get_number(array, 1);
		number.z = json_array_get_number(array, 2);

		return number;
	}
	else
		return default;
}

Quat JsonObj::GetQuaternion(const char* name, Quat default)
{
	if (json_object_has_value(object, name) == 1)
	{
		Quat quaternion;

		JSON_Array* array;
		array = json_object_get_array(object, name);

		quaternion.x = json_array_get_number(array, 0);
		quaternion.y = json_array_get_number(array, 1);
		quaternion.z = json_array_get_number(array, 2);
		quaternion.w = json_array_get_number(array, 3);

		return quaternion;
	}
	else
		return default;
}

void JsonObj::AddInt(const char* name, int number)
{
	json_object_set_number(object, name, number);
}

void JsonObj::AddFloat(const char* name, float number)
{
	json_object_set_number(object, name, number);
}

void JsonObj::AddFloat3(const char* name, float3 number)
{
	JSON_Value* value = json_value_init_array();
	JSON_Status status = json_object_set_value(object, name, value);
	JSON_Array* _array = json_object_get_array(object, name);

	json_array_append_number(_array, number.x);
	json_array_append_number(_array, number.y);
	json_array_append_number(_array, number.z);
}

void JsonObj::AddQuaternion(const char* name, Quat number)
{
	JSON_Value* value = json_value_init_array();
	JSON_Status status = json_object_set_value(object, name, value);
	JSON_Array* _array = json_object_get_array(object, name);

	json_array_append_number(_array, number.x);
	json_array_append_number(_array, number.y);
	json_array_append_number(_array, number.z);
	json_array_append_number(_array, number.w);
}

void JsonObj::AddBool(const char* name, bool boolean)
{
	json_object_set_boolean(object, name, boolean);
}

void JsonObj::AddString(const char* name, const char* string)
{
	json_object_set_string(object, name, string);
}

JsonArray JsonObj::AddArray(const char* name)
{
	JSON_Value* value = json_value_init_array();
	JSON_Status status = json_object_set_value(object, name, value);

	if (status == JSONFailure)
	{
		json_value_free(value);
	}
	else
	{
		return JsonArray(json_object_get_array(object, name));
	}
}

JsonArray JsonObj::AddArray(JsonArray array)
{
	return JsonArray();
}

//JsonArray =====================================================

JsonArray::JsonArray() : _array(nullptr), _nested(false)
{
	_value = json_value_init_array();
	json_value_get_array(_value);
}

JsonArray::JsonArray(JSON_Array * array, JSON_Object * root_object) : _array(array) {}

JsonArray::JsonArray(JSON_Array * array) : _array(array), _value(nullptr) {}

JsonArray::JsonArray(const char* name) : _nested(false)
{
	JSON_Value* value = json_value_init_array();
	json_value_get_array(value);
}

JsonArray::~JsonArray()
{
	_array = nullptr;
}

int JsonArray::Size()
{
	return json_array_get_count(_array);
}

JsonObj JsonArray::GetObjectInArray(const char* name)
{
	for (size_t i = 0; i < json_array_get_count(_array); i++)
	{
		JSON_Object* object = json_array_get_object(_array, i);
		const char* object_name = json_object_get_string(object, "name");

		if (strcmp(name, object_name) == 0)
			return JsonObj(object);
	}

	LOG("JSON object %s could not be found", name);
}

JsonObj JsonArray::GetObjectAt(int index)
{
	JSON_Object* object = json_array_get_object(_array, index);
	return JsonObj(object);
}

void JsonArray::AddObject(JsonObj & object)
{
	json_array_append_value(_array, object.GetValue());
}

void JsonArray::AddInt(int number)
{
	json_array_append_number(_array, number);
}

void JsonArray::AddFloat(float number)
{
	json_array_append_number(_array, number);
}

void JsonArray::AddString(const char* string)
{
	json_array_append_string(_array, string);
}