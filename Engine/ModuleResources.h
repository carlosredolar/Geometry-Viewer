#pragma once
#include "Globals.h"
#include "Module.h"
#include "Resource.h"
#include "Importers.h"

#include <map>
#include <string>

class GameObject;
struct AssetsIcons;

struct ResourceData
{
	std::string name;
	std::string assetsFile;
	std::string libraryFile;
	ResourceType type = ResourceType::RESOURCE_UNKNOWN;
};

class ModuleResources : public Module 
{
public:
	ModuleResources(bool startEnabled = true);
	~ModuleResources();

	bool Init() override;
	bool CleanUp() override;
	void OnGUI();
	void OnFrameEnd() override;

	void LoadEngineAssets(AssetsIcons& icons);
	bool CheckMeta(const char* assets_file, const char* meta_file);
	int GetUIDFromMeta(const char* meta_file);
	int Find(const char* assets_file);
	const char* Find(uint UID);
	const char* GetLibraryPath(uint UID);
	bool ExistsResource(uint UID);

	uint ImportFile(const char* assets_file);
	uint ImportInternalResource(const char* path, const void* data, ResourceType type, uint UID = 0);
	uint ReimportFile(const char* assets_file);

	void CreateResourceData(uint UID, const char* name = "No Name", const char* assets_path = "No path", const char* library_path = "No path");
	void DragAndDropFile(const char* path);

	void AssetToDelete(const char* asset_path);
	void ResourceToDelete(uint UID);
	bool DeleteAsset(const char* assets_path);
	bool DeleteResource(uint UID);
	bool DeleteInternalResource(uint UID);
	bool DeleteInternalResources(uint UID);

	Resource* CreateResource(const char* assetsPath, ResourceType type, uint UID = 0);
	Resource* CreateResource(uint UID, ResourceType type, std::string assets_file = "");
	Resource* RequestResource(uint UID);
	ResourceData RequestResourceData(uint UID);
	GameObject* RequestGameObject(const char* assets_file);

	Resource* LoadResource(uint UID, ResourceType type);
	void UnloadResource(Resource* resource);
	bool SaveResource(Resource* resource);
	bool SaveMetaFile(Resource* resource);
	bool LoadMetaFile(Resource* resource);
	void ReleaseResource(uint UID);
	void ReleaseResourceData(uint UID);

	uint GenerateUID();
	ResourceType GetTypeFromPath(const char* path);
	const char* GenerateLibraryPath(Resource* resource);
	std::string GenerateLibraryPath(uint uid, ResourceType type);
	std::string GetLibraryFolder(const char* file_in_assets);
	const char* GenerateAssetsPath(const char* path);
	std::string GenerateMetaFile(const char* assets_path);
	void AddFileExtension(std::string& file, ResourceType type);

public:
	ModelImportingSettings modelImportSettings;
	TextureImportingSettings textureImportSettings;

private:
	void CheckAssets(const char* directory);

private:
	std::map<uint, Resource*> resources;
	std::map<uint, ResourceData> resources_data;
	int _toDeleteAsset;
	int _toDeleteResource;
};