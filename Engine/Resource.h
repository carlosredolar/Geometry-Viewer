#pragma once
#include "Globals.h"

#include <string>

class JsonObj;

enum ResourceType
{
	RESOURCE_MODEL,
	RESOURCE_MESH,
	RESOURCE_MATERIAL,
	RESOURCE_TEXTURE,
	RESOURCE_SCENE,
	RESOURCE_UNKNOWN
};

class Resource
{
public:
	Resource(uint uid, ResourceType type);
	virtual ~Resource();

	ResourceType GetType();
	uint GetUID();

	virtual uint Save(JsonObj& baseObject) { return -1; };
	virtual uint SaveMeta(JsonObj& baseObject, uint last_modification) { return -1; };
	virtual void Load(JsonObj& baseObject) {};

public:
	std::string assetsFile;
	std::string libraryFile;

	uint referenceCount = 0;

protected:
	uint _uid = 0;

	ResourceType _type = RESOURCE_UNKNOWN;
};

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(JsonObj& baseObject);

public:
	uint amountVertices = -1;
	float* vertices = nullptr;

	uint amountIndices = -1;
	uint* indices = nullptr;

	uint amountNormals = -1;
	float* normals;

	uint amountTextureCoords = -1;
	float* textureCoords = nullptr;

	float* colors;
};

typedef unsigned char GLubyte;

class ResourceTexture : public Resource {
public:
	ResourceTexture(uint UID);
	~ResourceTexture();

	uint SaveMeta(JsonObj& baseObject, uint last_modification) override;
	//void Load(JsonObj& baseObject) override;

public:
	uint id;
	int width;
	int height;
	GLubyte* data;
};
