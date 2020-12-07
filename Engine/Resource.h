#pragma once
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <string>

class JsonObj;

enum ResourceType
{
	RESOURCE_SCENE,
	RESOURCE_MODEL,
	RESOURCE_MESH,
	RESOURCE_MATERIAL,
	RESOURCE_TEXTURE,
	RESOURCE_UNKNOWN
};

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

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(JsonObj& baseObject);

	void GenerateBuffers();
	void DeleteBuffers();

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

private:
	bool _buffers_created;
};

typedef unsigned char GLubyte;

class ResourceTexture : public Resource {
public:
	ResourceTexture(uint UID);
	~ResourceTexture();

	uint SaveMeta(JsonObj& baseObject, uint last_modification) override;
	void Load(JsonObj& baseObject) override;

	void GenerateBuffers();
	void BindTexture();

	uint GetID();
	int GeWidth();
	int GetHeight();
	GLubyte* GetData();
	uint GetGpuID();

private:
	uint id;
	int width;
	int height;
	GLubyte* data;
	uint gpuID;
};

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
	//std::vector<Light> lights;
	//std::vector<Camera> cameras;
};

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	bool Init();
	bool LoadConfig(JsonObj& config) override;
	update_status Update(float dt);
	void HandleInput();
	bool CleanUp();

	void AddGameObject(GameObject* gameObject);
	void DeleteGameObject(GameObject* gameObject);
	GameObject* GetRoot() { return root; }
	std::vector<GameObject*> GetAllGameObjects();
	void PreorderGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects);
	void EditTransform();

	bool ClearScene();

	bool Save(const char* file_path);
	bool Load(const char* scene_file);

public:
	bool show_grid;
	GameObject* selectedGameObject;

private:
	GameObject* root;
	ImGuizmo::OPERATION mCurrentGizmoOperation;
	ImGuizmo::MODE mCurrentGizmoMode;
};