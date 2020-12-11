#pragma once
#include "Globals.h"
#include "Color.h"
#include "MathGeoLib/include/MathGeoLib.h"

struct json_array_t;
typedef json_array_t JSON_Array;

struct json_value_t;
typedef json_value_t JSON_Value;

struct json_object_t;
typedef json_object_t JSON_Object;

class JsonArray;

class JsonObj 
{
public:
	JsonObj();
	JsonObj(const char* buffer);
	JsonObj(JSON_Object* object);
	~JsonObj();

	JSON_Object* GetJSONObject();
	JSON_Value* GetValue();
	void Release();

	uint Save(char** buffer);

	JSON_Array* GetArray(const char* name);

	int GetInt(const char* name, int default = -1);
	float GetFloat(const char* name, float default = -1.0f);
	bool GetBool(const char* name, bool default = false);
	const char* GetString(const char* name, const char* default);
	float3 GetFloat3(const char* name, float3 default = float3(0,0,0));
	Quat GetQuaternion(const char* name, Quat default = Quat(0,0,0,1));
	Color GetColor(const char* name, Color default = Color(0, 0, 0, 1));

	void AddInt(const char* name, int number);
	void AddFloat(const char* name, float number);
	void AddFloat3(const char* name, float3 number);
	void AddQuaternion(const char* name, Quat number);
	void AddBool(const char* name, bool boolean);
	void AddString(const char* name, const char* string);
	void AddColor(const char* name, Color color);
	JsonArray AddArray(const char* name);
	JsonArray AddArray(JsonArray array);

private:
	JSON_Object* objectObj;
	JSON_Value* rootObj;
};

class JsonArray 
{
public:
	JsonArray();
	JsonArray(JSON_Array* array);
	JsonArray(JSON_Array* array, JSON_Object* root_object);
	JsonArray(const char* name);
	~JsonArray();

	int Size();
	JsonObj GetObjectInArray(const char* name);
	JsonObj GetObjectAt(int index);

	void AddObject(JsonObj& object);
	void AddInt(int number);
	void AddFloat(float number);
	void AddString(const char* string);

private: 
	JSON_Array* _array;
	JSON_Value* _value;
	bool _nested;
};
