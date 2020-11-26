#pragma once
#include "Resource.h"

class JsonObj;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(JsonObj& objectToLoad);

public:
	uint amountVertices = -1;
	float* vertices = nullptr;

	uint amountIndices = -1;
	uint* indices = nullptr;

	float* normals;
	uint amountNormals = -1;

	float* textureCoords = nullptr;
	uint amountTextureCoords = -1;

	float* colors;
};