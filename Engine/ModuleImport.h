#pragma once
#include "Globals.h"
#include "Module.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "Assimp/include/scene.h"

class GameObject;

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
	bool LoadVertexNormalsTexturesIndex(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords, std::vector<uint>& indices);
};