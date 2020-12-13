#pragma once
#include "Resource.h"

class JsonObj;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(JsonObj& loadingObj);

	void GenerateBuffers();
	void DeleteBuffers();

public:
	uint verticesBuffer = 0;
	uint amountVertices = 0;
	float* vertices = nullptr;

	uint indicesBuffer = 0;
	uint amountIndices = 0;
	uint* indices = nullptr;

	uint normalsBuffer = 0;
	uint amountNormals = 0;
	float* normals;

	uint texcoordsBuffer = 0;
	uint amountTexCoords = 0;
	float* textureCoords = nullptr;

	float* colors = nullptr;;

private:
	bool buffersCreated;
};