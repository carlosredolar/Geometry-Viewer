#pragma once
#include "Globals.h"
#include "Module.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "Assimp/include/scene.h"

class Resource;
class ResourceModel;
class ResourceMesh;
class ResourceMaterial;
class ResourceTexture;

struct ModelNode;
class JsonArray;
class GameObject;
struct Component_Material;
class aiMesh;
class Component_Mesh;
class Transform;
class Material;

namespace ModelImporter
{
	void Import(char* fileBuffer, ResourceModel* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, ResourceModel* model);
	void ReimportFile(char* fileBuffer, ResourceModel* resource, uint size);
	uint64 Save(ResourceModel* model, char** fileBuffer);
	void LoadTransform(aiNode* node, ModelNode& modelNode);
	bool Load(char* fileBuffer, ResourceModel* model, uint size);

	GameObject* ConvertToGameObject(ResourceModel* model);
	void ExtractInternalResources(const char* path, std::vector<uint>& meshes, std::vector<uint>& materials);
	void ExtractInternalResources(const char* meta_file, ResourceModel& model);
	bool InternalResourcesExist(const char* path);
	void ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode);
}

namespace MeshImporter
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, ResourceMesh* mesh);
	uint64 Save(ResourceMesh* mesh, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceMesh* mesh, uint size);
}

namespace TextureImporter
{
	void Init();

	void Import(char* fileBuffer, ResourceTexture* resource, uint size);
	uint Save(ResourceTexture* texture, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceTexture* texture, uint size);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
	void ApplyImportingOptions(TextureImportingOptions importingOptions);
}

namespace MaterialImporter
{
	void Import(const aiMaterial* aimaterial, ResourceMaterial* material);
	uint64 Save(ResourceMaterial* material, char** fileBuffer);
	bool Load(const char* fileBuffer, ResourceMaterial* material, uint size);
	bool Unload(uint imageID);

	bool DeleteTexture(const char* material_library_path);
	const char* ExtractTexture(const char* material_library_path);
}