#ifndef _IMPORTERS_H_
#define _IMPORTERS_H_

#include "Globals.h"
#include "ImportingSettings.h"
#include <vector>
#include <string>
#include "MathGeoLib/include/MathGeoLib.h"

//Model Importer

class Resource;
class ResourceModel;
class ResourceMesh;

struct ModelNode;
class JsonArray;
class GameObject;
struct Component_Texture;
class Component_Mesh;
class Component_Transform;
class Component_Material;

class aiMesh;
class aiScene;
struct aiNode;
class aiMaterial;

namespace ModelImporter
{
	void Import(char* fileBuffer, ResourceModel* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceModel* model);
	void AddParentTransform(ModelNode* node, ModelNode* parentNode);
	void LoadTransform(aiNode* node, ModelNode& modelNode);
	void ReimportFile(char* fileBuffer, ResourceModel* resource, uint size);
	uint64 Save(ResourceModel* model, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceModel* model, uint size);

	GameObject* ConvertToGameObject(ResourceModel* model);
	void ExtractInternalResources(const char* path, std::vector<uint>& meshes, std::vector<uint>& materials);
	void ExtractInternalResources(const char* meta_file, ResourceModel& model);
	bool InternalResourcesExist(const char* path);
	void ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode);
}

//Mesh Importer

class ResourceMesh;

class aiMesh;

namespace MeshImporter
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, ResourceMesh* mesh);
	uint64 Save(ResourceMesh* mesh, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceMesh* mesh, uint size);
}

//Material Importer
class aiMaterial;
class ResourceMaterial;

namespace MaterialImporter
{
	void Import(const aiMaterial* aimaterial, ResourceMaterial* material);
	uint64 Save(ResourceMaterial* material, char** fileBuffer);
	bool Load(const char* fileBuffer, ResourceMaterial* material, uint size);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	bool DeleteTexture(const char* material_library_path);
	const char* ExtractTexture(const char* material_library_path);
}

//Texture Importer
struct TextureImportSettings;
class ResourceTexture;

typedef unsigned int ILenum;

namespace TextureImporter
{
	void Init();

	void Import(char* fileBuffer, ResourceTexture* resource, uint size);
	uint Save(ResourceTexture* texture, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceTexture* texture, uint size);

	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
	void ApplyImportingOptions(TextureImportSettings importingOptions);
}

#endif // !_IMPORTERS_H_


