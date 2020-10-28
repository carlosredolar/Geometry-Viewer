#pragma once
#include "Globals.h"
#include "Module.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "Assimp/include/scene.h"

class GameObject;

struct mesh
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	bool enabled = true;
};

class ModuleImport : public Module
{
public:
	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	bool Init();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	uint LoadTexture(const char* path);
	uint LoadDefaultTexture();

	void LoadMesh(char* filepath);
	bool LoadSceneMeshes(const aiScene* scene, const aiNode* parent, GameObject* gOParent);
	bool LoadNodeMeshes(const aiScene* scene, const aiNode* node, GameObject* parent);
	bool LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords);
	bool LoadIndices(aiMesh* mesh, std::vector<uint>& indices);
	void RenderMesh(mesh* m);

	std::vector<mesh> meshes;

	uint vertex_buffer = 0;
	uint index_buffer = 0;
};