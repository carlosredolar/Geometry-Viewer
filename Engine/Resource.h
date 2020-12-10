#pragma once
#include "Globals.h"
#include "ImGui/imgui.h"
#include <string>
#include "Color.h"
#include "ModuleJson.h"

enum ResourceType
{
	RESOURCE_SCENE,
	RESOURCE_MODEL,
	RESOURCE_MESH,
	RESOURCE_MATERIAL,
	RESOURCE_TEXTURE,
	RESOURCE_UNKNOWN
};

// Base Resource class

class Resource
{
public:
	Resource(uint uid, ResourceType type);
	virtual ~Resource();

	virtual uint Save(JsonObj& baseObject) { return -1; };
	virtual uint SaveMeta(JsonObj& baseObject, uint last_modification) { return -1; };
	virtual void Load(JsonObj& baseObject);

	ResourceType GetType();
	uint GetUID();

public:
	std::string name;
	std::string assetsFile;
	std::string libraryFile;

	uint referenceCount = 0;

protected:
	uint uid = 0;

	ResourceType type = RESOURCE_UNKNOWN;
};

//Resource Mesh

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(JsonObj& baseObject);

	void GenerateBuffers();
	void DeleteBuffers();

public:
	uint verticesBuffer = 0;
	uint amountVertices = -1;
	float* vertices = nullptr;

	uint indicesBuffer = 0;
	uint amountIndices = -1;
	uint* indices = nullptr;

	uint normalsBuffer = 0;
	uint amountNormals = -1;
	float* normals = nullptr;

	uint textureCoordsBuffer = 0;
	uint amountTextureCoords = -1;
	float* textureCoords = nullptr;

	//uint idVertex;
	//std::vector<float3> vertices;

	//uint idIndex;
	//std::vector<uint> indices;

	//uint idNormals;
	//std::vector<float3> normals;

	//uint idTextureCoords;
	//std::vector<float2> textureCoords;

	float* colors;

private:
	bool buffersCreated;
};

typedef unsigned char GLubyte;

// Resource Texture

class ResourceTexture : public Resource {
public:
	ResourceTexture(uint UID);
	~ResourceTexture();

	void GenerateBuffers();
	void BindTexture();

	void FillData(GLubyte* data, uint UUID, int width, int height);
	uint SaveMeta(JsonObj& base_object, uint last_modification) override;
	void Load(JsonObj& base_object) override;

	uint GetID();
	int GetWidth();
	int GetHeight();
	GLubyte* GetData();
	uint GetGpuID();
	std::string GetPath();

private:
	uint uuid;
	int width;
	int height;
	GLubyte* data;
	uint gpuID;
	std::string path;
};

// Resource Model

class Light;

struct ModelNode
{
	std::string name;
	float3 position;
	Quat rotation;
	float3 scale;

	int meshID = -1;
	int materialID = -1;

	uint UID;
	uint parentUID;
};

class ResourceModel : public Resource {
public:
	ResourceModel(uint UID);
	~ResourceModel();

	uint Save(JsonObj& base_object) override;
	uint SaveMeta(JsonObj& base_object, uint last_modification) override;

public:
	std::vector<ModelNode> nodes;
	std::vector<uint> meshes;
	std::vector<uint> materials;
	std::vector<uint> textures;
	std::vector<Light*> lights;
};

// Resource Material

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(uint UID);
	~ResourceMaterial();

public:
	uint diffuseTextureUID;
	Color diffuseColor;
};