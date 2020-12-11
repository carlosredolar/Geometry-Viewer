#pragma once
#include "Resource.h"

class JsonObj;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(JsonObj& base_object);

	void GenerateBuffers();
	void DeleteBuffers();

public:
	uint vertices_buffer = 0;
	uint vertices_amount = 0;
	float* vertices = nullptr;

	uint indices_buffer = 0;
	uint indices_amount = 0;
	uint* indices = nullptr;

	uint normals_buffer = 0;
	uint normals_amount = 0;
	float* normals;

	uint texcoords_buffer = 0;
	uint texcoords_amount = 0;
	float* texcoords = nullptr;

	float* colors = nullptr;;

private:
	bool _buffers_created;
};