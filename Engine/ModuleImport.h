#pragma once
#include "Globals.h"

#include <vector>
#include <string>

class Resource;
class ResourceModel;
class ResourceMesh;
class ResourceMaterial;
class ResourceTexture;

struct TextureImportingSettings;
struct ModelNode;
class JsonArray;
class GameObject;
class Component_Mesh;
class Component_Transform;
class Component_Material;

class aiMesh;
class aiScene;
struct aiNode;
class aiMaterial;

typedef unsigned int ILenum;

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
	ILenum ExtractFileExtension(const char* file);
	void ApplyImportingSettings(TextureImportingSettings importingSettings);
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

// Importing Options

enum Axis
{
	X,
	Y,
	Z,
	MINUS_X,
	MINUS_Y,
	MINUS_Z
};

struct ModelImportingSettings
{
	float globalScale = 1.0f;
	Axis forwardAxis = Axis::Z;
	Axis upAxis = Axis::Y;
	bool normalizeScales = true;
	bool ignoreCameras = true;
	bool ignoreLights = true;
};

enum class TextureWrap
{
	CLAMP_TO_BORDER,
	CLAMP,
	REPEAT,
	MIRRORED_REPEAT
};

enum class TextureFiltering
{
	NEAREST,
	LINEAR
};

struct TextureImportingSettings
{
	TextureWrap textureWrap = TextureWrap::REPEAT;
	TextureFiltering textureFiltering = TextureFiltering::NEAREST;
	bool flip = false;
	bool alienify = false;
	bool blur_average = false;
	bool blur_gaussian = false;
	bool equalize = false;
	bool negativity = false;
	bool noise = false;
	bool pixelize = false;
	int pixelize_size = 1;
	bool sharpening = false;
	float sharpening_factor = 1.0f;
	int sharpening_iterations = 1;
	float noise_tolerance = 0.5f;
	float gamma_correction = 1.0f;
	float contrast = 1.0f;
};