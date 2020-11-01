#pragma once
#include "Globals.h"
#include "Module.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "Assimp/include/scene.h"

class GameObject;

struct meshInfo 
{
	std::string name;

	uint idVertex;
	std::vector<float3> vertices;

	uint idIndex;
	std::vector<uint> indices;

	uint idNormals;
	std::vector<float3> normals;

	uint idTextureCoords;
	std::vector<float2> textureCoords;
};

struct textureInfo
{
	std::string name;

	std::string path;

	uint id;

	uint checkerTexture;

	int w, h;
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

	void ImportExternalFiles(const char* path);
	void ExtensionClassifier(const char* path);

	void LoadMesh(const char* filepath);
	bool LoadSceneMeshes(const aiScene* scene, const aiNode* parent, GameObject* gOParent);
	bool LoadNodeMeshes(const aiScene* scene, const aiNode* node, GameObject* parent);
	bool LoadVertexNormalsTexturesIndex(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords, std::vector<uint>& indices);
	std::vector<meshInfo*> meshes;
	
	textureInfo* LoadTexture(const char* path);
	uint LoadDefaultTexture();
	std::vector<textureInfo*> textures;

	uint checkerTexture;
};