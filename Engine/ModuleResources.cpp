#include "ModuleResources.h"
#include "GameObject.h"
#include "FileManager.h"
#include "ModuleImport.h"
#include "ModuleScene.h"
#include "Application.h"

#include "ModuleJson.h"
#include "Resource.h"

#include "GuiImport.h"
#include <algorithm>

#include "MathGeoLib/include/MathGeoLib.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
	name = "resources";
}

ModuleResources::~ModuleResources() {}

bool ModuleResources::Init()
{
	bool ret = true;

	MeshImporter::Init();
	TextureImporter::Init();

	//std::vector<std::string> files;
	//std::vector<std::string> dirs;
	//App->fileManager->DiscoverFilesRecursive("Library", files, dirs);

	CheckAssetsRecursive("Assets");

	//LoadEngineAssets();

	return ret;
}

bool ModuleResources::CleanUp()
{
	bool ret = true;

	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		ReleaseResource(it->second->GetUID());
	}

	resources.clear();
	resources_data.clear();

	return ret;
}

void ModuleResources::OnGUI()
{
	std::vector<Resource*> meshes;
	std::vector<Resource*> materials;
	std::vector<Resource*> textures;

	std::map<uint, Resource*>::iterator it = resources.begin();
	for (it; it != resources.end(); it++)
	{
		switch (it->second->GetType())
		{
		case ResourceType::RESOURCE_MESH:
			meshes.push_back(it->second);
			break;
		case ResourceType::RESOURCE_MATERIAL:
			materials.push_back(it->second);
			break;
		case ResourceType::RESOURCE_TEXTURE:
			textures.push_back(it->second);
			break;
		default:
			break;
		}
	}

	if (ImGui::TreeNode("Meshes")) {
		ImGui::Separator();
		for (size_t i = 0; i < meshes.size(); i++)
		{
			ImGui::Text("UID: %d", meshes[i]->GetUID());
			ImGui::Text("Assets path: %s", meshes[i]->assetsFile.c_str());
			ImGui::Text("Library path: %s", meshes[i]->libraryFile.c_str());
			ImGui::Text("Reference count: %d", meshes[i]->referenceCount);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Materials")) {
		for (size_t i = 0; i < materials.size(); i++)
		{
			ImGui::Text("UID: %d", materials[i]->GetUID());
			ImGui::Text("Assets path: %s", materials[i]->assetsFile.c_str());
			ImGui::Text("Library path: %s", materials[i]->libraryFile.c_str());
			ImGui::Text("Reference count: %d", materials[i]->referenceCount);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Textures")) {
		for (size_t i = 0; i < textures.size(); i++)
		{
			ImGui::Text("UID: %d", textures[i]->GetUID());
			ImGui::Text("Assets path: %s", textures[i]->assetsFile.c_str());
			ImGui::Text("Library path: %s", textures[i]->libraryFile.c_str());
			ImGui::Text("Reference count: %d", textures[i]->referenceCount);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::TreePop();
	}
}

void ModuleResources::LoadEngineAssets()
{
	ResourceTexture* folder_tex = dynamic_cast<ResourceTexture*>(RequestResource(Find("Assets/EngineAssets/folder.png")));
}

update_status ModuleResources::OnFrameEnd()
{
	bool ret = true;
	if (_toDeleteResource != -1)
	{
		ret = DeleteResource(_toDeleteResource);
		_toDeleteResource = -1;
	}

	if (_toDeleteAsset != -1)
	{
		std::string assets_path = resources_data[_toDeleteAsset].assetsFile;
		ret = DeleteResource(_toDeleteAsset);
		ret = DeleteAsset(assets_path.c_str());
		_toDeleteAsset = -1;
	}

	if (ret) return UPDATE_CONTINUE;
	else return UPDATE_ERROR;
}

bool ModuleResources::MetaUpToDate(const char* assets_file, const char* meta_file)
{
	bool ret = true;

	char* buffer = nullptr;
	uint size = App->fileManager->Load(meta_file, &buffer);
	JsonObj meta(buffer);

	uint UID = meta.GetInt("UID");
	int lastModifiedMeta = meta.GetInt("lastModified");
	uint lastModified = App->fileManager->GetLastModTime(assets_file);

	if (lastModifiedMeta == lastModified)
	{
		std::string library_path = meta.GetString("Library path", "NoPath");

		if (library_path == "NoPath")
			library_path = Find(UID);

		//check for the file itself to exist
		if (!App->fileManager->Exists(library_path.c_str()))
		{
			ret = false;
		}
		//check its internal resources
		else if (GetTypeFromPath(assets_file) == ResourceType::RESOURCE_MODEL)
		{
			ret = ModelImporter::InternalResourcesExist(meta_file);
		}

		resources_data[UID].assetsFile = assets_file;
		resources_data[UID].libraryFile = library_path;
		resources_data[UID].type = GetTypeFromPath(assets_file);
	}
	else
	{
		ret = false;
	}

	meta.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

int ModuleResources::GetUIDFromMeta(const char* meta_file)
{
	char* buffer = nullptr;
	uint size = App->fileManager->Load(meta_file, &buffer);
	JsonObj meta(buffer);

	int UID = meta.GetInt("UID", -1);

	meta.Release();
	RELEASE_ARRAY(buffer);

	return UID;
}

int ModuleResources::Find(const char* assets_file)
{
	int UID = -1;

	std::map<uint, Resource*>::iterator resource_it = resources.begin();
	std::map<uint, ResourceData>::iterator resources_data_it = resources_data.begin();

	//First we loop through all loaded resources
	for (resource_it; resource_it != resources.end(); resource_it++) {
		if (resource_it->second->assetsFile == App->fileManager->LowerCaseString(assets_file))
			return resource_it->first;
	}

	//If not found we loop through all not loaded but known resources
	for (resources_data_it; resources_data_it != resources_data.end(); resources_data_it++) {
		if (resources_data_it->second.assetsFile == App->fileManager->LowerCaseString(assets_file))
			return resources_data_it->first;
	}

	return UID;
}

const char* ModuleResources::Find(uint UID)
{
	if (resources_data.find(UID) != resources_data.end() && resources_data[UID].libraryFile.size() > 0)
		return resources_data[UID].libraryFile.c_str();

	std::vector<std::string> directories = { "Library/Config/","Library/Models/","Library/Meshes/","Library/Materials/","Library/Textures/", "Library/Scenes/" };
	std::vector<std::string> extensions = { ".json",".modelr",".meshr",".materialr",".texturer", ".scener" };

	for (size_t i = 0; i < directories.size(); i++)
	{
		std::string file = directories[i];
		file += std::to_string(UID);
		file += extensions[i];
		if (App->fileManager->Exists(file.c_str()))
		{
			char* final_file = new char[256];
			sprintf_s(final_file, 256, file.c_str());
			std::string library_path = final_file;
			resources_data[UID].libraryFile = library_path;
			return final_file;
		}
	}

	return nullptr;
}

const char* ModuleResources::GetLibraryPath(uint UID)
{
	std::map<uint, Resource*>::iterator resources_it = resources.find(UID);
	std::map<uint, ResourceData>::iterator resources_data_it = resources_data.find(UID);

	if (resources_it != resources.end()) {
		return resources_it->second->libraryFile.c_str();
	}

	if (resources_data_it != resources_data.end()) {
		return resources_data_it->second.libraryFile.c_str();
	}
}

bool ModuleResources::Exists(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end())
		return true;
	else
		return false;
}



uint ModuleResources::ImportFile(const char* assets_file)
{
	/*
	std::string meta_file = assets_file;
	meta_file.append(".meta");
	if(App->fileManager->Exists(meta_file.c_str()))
		return GetUIDFromMeta(meta_file.c_str());
	*/

	ResourceType type = GetTypeFromPath(assets_file);

	Resource* resource = CreateResource(assets_file, type);
	uint ret = 0;

	char* fileBuffer;
	uint size = App->fileManager->Load(assets_file, &fileBuffer);

	switch (type)
	{
	case RESOURCE_MODEL:
		ModelImporter::Import(fileBuffer, (ResourceModel*)resource, size);
		break;
	case RESOURCE_TEXTURE:
		TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size);
		break;
	case RESOURCE_SCENE:
		break;
	default:
		WARNING_LOG("Trying to import unknown file: %s", assets_file);
		break;
	}

	if (resource == nullptr) {
		ERROR_LOG("Fatal error when importing file: %s", assets_file);
		return 0;
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
}

uint ModuleResources::ImportInternalResource(const char* path, const void* data, ResourceType type, uint UID)
{
	uint ret = 0;
	Resource* resource = CreateResource(path, type, UID);

	switch (type)
	{
	case RESOURCE_MESH:
		MeshImporter::Import((aiMesh*)data, (ResourceMesh*)resource);
		break;
	case RESOURCE_MATERIAL:
		MaterialImporter::Import((aiMaterial*)data, (ResourceMaterial*)resource);
		break;
	default:
		break;
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);

	return ret;
}

uint ModuleResources::ReimportFile(const char* assets_file)
{
	WARNING_LOG("Reimporting file: %s", assets_file);

	std::string meta_file = assets_file;
	meta_file.append(".meta");

	uint ret = GetUIDFromMeta(meta_file.c_str());
	ResourceType type = GetTypeFromPath(assets_file);
	Resource* resource = CreateResource(assets_file, type, ret);

	char* fileBuffer;
	uint size = App->fileManager->Load(assets_file, &fileBuffer);

	if (size <= 0)
	{
		ERROR_LOG("Trying to load unexisting file: %s", assets_file);
		return 0;
	}

	switch (type)
	{
	case ResourceType::RESOURCE_MODEL:
		ModelImporter::ReimportFile(fileBuffer, (ResourceModel*)resource, size);
		break;
	case ResourceType::RESOURCE_TEXTURE:
		if (App->fileManager->Exists(resource->libraryFile.c_str()))
			App->fileManager->Delete(resource->libraryFile.c_str());

		TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size);
		break;
	default:
		break;
	}

	if (resource == nullptr)
	{
		ERROR_LOG("Fatal error when reimporting file: %s", assets_file);
		return 0;
	}
	else
	{
		LOG("File: %s reimported successfully", assets_file);
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
}

void ModuleResources::CreateResourceData(uint UID, const char* name, const char* assets_path, const char* library_path)
{
	resources_data[UID].name = name;
	resources_data[UID].assetsFile = assets_path;
	resources_data[UID].libraryFile = library_path;
	resources_data[UID].type = GetTypeFromPath(library_path);
}

void ModuleResources::DragDropFile(const char* path)
{
	std::string file_to_import = path;

	if (!App->fileManager->Exists(path))
	{
		file_to_import = GenerateAssetsPath(path);
		App->fileManager->DuplicateFile(path, file_to_import.c_str());
	}

	char* final_path = new char[sizeof(char) * file_to_import.size()];
	strcpy(final_path, file_to_import.c_str());
	GuiImport* import_window = dynamic_cast<GuiImport*>(App->gui->ui_windows[4]);
	import_window->Enable(final_path, GetTypeFromPath(path));
}

void ModuleResources::AddAssetToDelete(const char* asset_path)
{
	std::string meta_file = asset_path;
	meta_file.append(".meta");
	_toDeleteAsset = GetUIDFromMeta(meta_file.c_str());
	App->AddModuleToTaskStack(this);
}

void ModuleResources::AddResourceToDelete(uint UID)
{
	_toDeleteResource = UID;
	App->AddModuleToTaskStack(this);
}

bool ModuleResources::DeleteAsset(const char* assets_path)
{
	bool ret = true;

	App->fileManager->Delete(assets_path);

	std::string meta_file = assets_path;
	meta_file.append(".meta");

	if (App->fileManager->Exists(meta_file.c_str()))
		App->fileManager->Delete(meta_file.c_str());

	return ret;
}

bool ModuleResources::DeleteResource(uint UID)
{
	bool ret = true;

	switch (resources_data[UID].type)
	{
	case ResourceType::RESOURCE_MODEL:
		DeleteInternalResources(UID);
		break;
	case ResourceType::RESOURCE_TEXTURE:
		break;
	default:
		break;
	}

	ReleaseResource(UID);
	App->fileManager->Delete(resources_data[UID].libraryFile.c_str());
	ReleaseResourceData(UID);

	return ret;
}

bool ModuleResources::DeleteInternalResources(uint UID)
{
	bool ret = true;
	std::string meta_file = resources_data[UID].assetsFile + ".meta";

	if (resources_data[UID].type == ResourceType::RESOURCE_MODEL)
	{
		std::vector<uint> meshes;
		std::vector<uint> materials;
		ModelImporter::ExtractInternalResources(resources_data[UID].libraryFile.c_str(), meshes, materials);

		for (size_t i = 0; i < meshes.size(); i++)
		{
			DeleteInternalResource(meshes[i]);
		}

		for (size_t i = 0; i < materials.size(); i++)
		{
			DeleteInternalResource(materials[i]);
		}
	}

	return ret;
}

bool ModuleResources::DeleteInternalResource(uint UID)
{
	bool ret = true;

	std::string library_path = Find(UID);
	ret = App->fileManager->Delete(library_path.c_str());

	ReleaseResource(UID);

	return ret;
}

Resource* ModuleResources::LoadResource(uint UID, ResourceType type)
{
	Resource* resource = CreateResource(UID, type, resources_data[UID].assetsFile.c_str());
	bool ret = true;

	char* buffer = nullptr;
	uint size = App->fileManager->Load(resource->libraryFile.c_str(), &buffer);

	if (size >= 0)
	{
		switch (resource->GetType())
		{
		case RESOURCE_MODEL:
			if (ModelImporter::InternalResourcesExist(resource->libraryFile.c_str()))
				ret = ModelImporter::Load(buffer, (ResourceModel*)resource, size);
			else {
				ReimportFile(resources_data[UID].assetsFile.c_str());
				ret = ModelImporter::Load(buffer, (ResourceModel*)resource, size);
			}
			break;
		case RESOURCE_MESH:
			ret = MeshImporter::Load(buffer, (ResourceMesh*)resource, size);
			break;
		case RESOURCE_MATERIAL:
			ret = MaterialImporter::Load(buffer, (ResourceMaterial*)resource, size);
			break;
		case RESOURCE_TEXTURE:
			ret = TextureImporter::Load(buffer, (ResourceTexture*)resource, size);
			LoadMetaFile(resource);
			break;
		case RESOURCE_SCENE:
			break;
		case RESOURCE_UNKNOWN:
			break;
		default:
			break;
		}
	}
	else
		ret = false;

	if (ret == false)
	{
		ERROR_LOG("Resource: %d could not be loaded");
		ReleaseResource(UID);
		resource = nullptr;
		return nullptr;
	}

	RELEASE_ARRAY(buffer);

	return resource;
}

void ModuleResources::UnloadResource(Resource * resource)
{
	switch (resource->GetType())
	{
	case ResourceType::RESOURCE_MODEL:
		break;
	case ResourceType::RESOURCE_MESH:
		break;
	case ResourceType::RESOURCE_MATERIAL:
		break;
	case ResourceType::RESOURCE_TEXTURE:
		TextureImporter::UnloadTexture(((ResourceTexture*)resource)->GetID());
		break;
	default:
		break;
	}

	resources.erase(resources.find(resource->GetUID()));

	delete resource;
	resource = nullptr;
}

Resource* ModuleResources::CreateResource(const char* assetsPath, ResourceType type, uint UID)
{
	Resource* resource = nullptr;

	if (UID == 0)
		UID = GenerateUID();

	switch (type)
	{
	case RESOURCE_MODEL:
		resource = new ResourceModel(UID);
		break;
	case RESOURCE_MESH:
		resource = new ResourceMesh(UID);
		break;
	case RESOURCE_MATERIAL:
		resource = new ResourceMaterial(UID);
		break;
	case RESOURCE_TEXTURE:
		resource = new ResourceTexture(UID);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		resources[UID] = resource;
		resources[UID]->name = App->fileManager->ExtractFileNameAndExtension(assetsPath);
		resource->assetsFile = App->fileManager->LowerCaseString(assetsPath);
		resource->libraryFile = GenerateLibraryPath(resource);

		resources_data[UID].name = resources[UID]->name;
		resources_data[UID].assetsFile = resource->assetsFile;
		resources_data[UID].libraryFile = resource->libraryFile;
		resources_data[UID].type = type;
	}

	return resource;
}

Resource* ModuleResources::CreateResource(uint UID, ResourceType type, std::string assets_file)
{
	Resource* resource = nullptr;

	switch (type)
	{
	case RESOURCE_MODEL:
		resource = new ResourceModel(UID);
		break;
	case RESOURCE_MESH:
		resource = new ResourceMesh(UID);
		break;
	case RESOURCE_MATERIAL:
		resource = new ResourceMaterial(UID);
		break;
	case RESOURCE_TEXTURE:
		resource = new ResourceTexture(UID);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		resources[UID] = resource;
		resource->libraryFile = GenerateLibraryPath(resource);

		if (assets_file.size() > 0)
			resource->assetsFile = assets_file;
		else
			resource->assetsFile = resources_data[UID].assetsFile;
	}

	return resource;
}

Resource* ModuleResources::RequestResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end() && it->second != nullptr) {
		it->second->referenceCount++;
		return it->second;
	}

	const char* library_file = Find(UID);

	if (library_file == nullptr)
		return nullptr;

	ResourceType type = GetTypeFromPath(library_file);
	Resource * resource = LoadResource(UID, type);

	if (resource != nullptr)
		resource->referenceCount++;

	return resource;
}

ResourceData ModuleResources::RequestResourceData(uint UID)
{
	std::map<uint, ResourceData>::iterator it = resources_data.find(UID);

	if (it != resources_data.end())
		return it->second;
}

GameObject* ModuleResources::RequestGameObject(const char* assets_file)
{
	std::string meta_file = assets_file;
	meta_file.append(".meta");
	ResourceModel* model = (ResourceModel*)RequestResource(GetUIDFromMeta(meta_file.c_str()));

	if (model == nullptr)
	{
		model = (ResourceModel*)RequestResource(ReimportFile(assets_file));
	}

	return ModelImporter::ConvertToGameObject(model);
}

void ModuleResources::ReleaseResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		it->second->referenceCount--;

		if (it->second->referenceCount <= 0)
			UnloadResource(it->second);
	}
}

void ModuleResources::ReleaseResourceData(uint UID)
{
	resources_data.erase(resources_data.find(UID));
}

bool ModuleResources::SaveResource(Resource * resource)
{
	bool ret = true;

	char* buffer;
	uint size = 0;

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		size = ModelImporter::Save((ResourceModel*)resource, &buffer);
		break;
	case RESOURCE_MESH:
		size = MeshImporter::Save((ResourceMesh*)resource, &buffer);
		break;
	case RESOURCE_MATERIAL:
		size = MaterialImporter::Save((ResourceMaterial*)resource, &buffer);
		break;
	case RESOURCE_TEXTURE:
		size = TextureImporter::Save((ResourceTexture*)resource, &buffer);
		break;
	case RESOURCE_SCENE:
		break;
	default:
		break;
	}

	if (size > 0)
	{
		App->fileManager->Save(resource->libraryFile.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);
	}

	if (resource->GetType() == ResourceType::RESOURCE_MODEL || resource->GetType() == ResourceType::RESOURCE_TEXTURE)
		ret = SaveMetaFile(resource);

	return ret;
}

bool ModuleResources::SaveMetaFile(Resource * resource)
{
	JsonObj base_object;
	resource->SaveMeta(base_object, App->fileManager->GetLastModTime(resource->assetsFile.c_str()));

	char* meta_buffer = NULL;
	uint meta_size = base_object.Save(&meta_buffer);

	std::string meta_file_name = resource->assetsFile + ".meta";
	App->fileManager->Save(meta_file_name.c_str(), meta_buffer, meta_size);

	base_object.Release();
	RELEASE_ARRAY(meta_buffer);

	return true;
}

bool ModuleResources::LoadMetaFile(Resource * resource)
{
	bool ret = true;

	std::string meta_file = resource->assetsFile;
	meta_file.append(".meta");

	char* buffer = nullptr;
	App->fileManager->Load(meta_file.c_str(), &buffer);

	JsonObj meta_data(buffer);

	resource->Load(meta_data);

	meta_data.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

ResourceType ModuleResources::GetTypeFromPath(const char* path)
{
	std::string extension = App->fileManager->ExtractFileExtension(path);

	if (extension == ".fbx" || extension == ".modelr")
		return ResourceType::RESOURCE_MODEL;

	else if (extension == ".meshr")
		return ResourceType::RESOURCE_MESH;

	else if (extension == ".materialr")
		return ResourceType::RESOURCE_MATERIAL;

	else if (extension == ".png" || extension == ".jpg" || extension == ".tga" || extension == ".texturer")
		return ResourceType::RESOURCE_TEXTURE;

	else
		return ResourceType::RESOURCE_UNKNOWN;
}

uint ModuleResources::GenerateUID()
{
	return LCG().Int();
}

const char* ModuleResources::GenerateLibraryPath(Resource * resource)
{
	char* library_path = new char[128];

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		sprintf_s(library_path, 128, "Library/Models/%d.modelr", resource->GetUID()); break;
	case RESOURCE_MESH:
		sprintf_s(library_path, 128, "Library/Meshes/%d.meshr", resource->GetUID()); break;
	case RESOURCE_MATERIAL:
		sprintf_s(library_path, 128, "Library/Materials/%d.materialr", resource->GetUID()); break;
	case RESOURCE_TEXTURE:
		sprintf_s(library_path, 128, "Library/Textures/%d.texturer", resource->GetUID()); break;
	case RESOURCE_SCENE:
		sprintf_s(library_path, 128, "Library/Scenes/%d.scener", resource->GetUID()); break;
	default:
		break;
	}

	return library_path;
}

std::string ModuleResources::GenerateLibraryPath(uint uid, ResourceType type)
{
	std::string path;
	switch (type)
	{
	case RESOURCE_MODEL:
		path = "Library/Models/";
		path.append(std::to_string(uid) + ".modelr");
		break;
	case RESOURCE_MESH:
		path = "Library/Meshes/";
		path.append(std::to_string(uid) + ".meshr");
		break;
	case RESOURCE_MATERIAL:
		path = "Library/Materials/";
		path.append(std::to_string(uid) + ".materialr");
		break;
	case RESOURCE_TEXTURE:
		path = "Library/Textures/";
		path.append(std::to_string(uid) + ".texturer");
		break;
	case RESOURCE_SCENE:
		path = "Library/Scenes/";
		path.append(std::to_string(uid) + ".scener");
		break;
	case RESOURCE_UNKNOWN:
		LOG("Error trying to generate a path for an unknown file %d", uid);
		break;
	default:
		break;
	}

	return path;
}

std::string ModuleResources::GetLibraryFolder(const char* file_in_assets)
{
	ResourceType type = GetTypeFromPath(file_in_assets);

	switch (type)
	{
	case RESOURCE_MODEL: return std::string("Library/Models/"); break;
	case RESOURCE_MESH: return std::string("Library/Meshes/"); break;
	case RESOURCE_MATERIAL: return std::string("Library/Materials/"); break;
	case RESOURCE_TEXTURE: return std::string("Library/Textures/");	break;
	case RESOURCE_SCENE: return std::string("Library/Scenes/");	break;
	default: break;
	}
}

const char* ModuleResources::GenerateAssetsPath(const char* path)
{
	ResourceType type = GetTypeFromPath(path);
	std::string file = App->fileManager->ExtractFileName(path);

	char* library_path = new char[128];

	switch (type)
	{
	case RESOURCE_MODEL:
		sprintf_s(library_path, 128, "Assets/Models/%s", file.c_str()); break;
	case RESOURCE_TEXTURE:
		sprintf_s(library_path, 128, "Assets/Textures/%s", file.c_str()); break;
	case RESOURCE_SCENE:
		sprintf_s(library_path, 128, "Assets/Scenes/%s", file.c_str()); break;
	default:
		break;
	}

	return library_path;
}

std::string ModuleResources::GenerateMetaFile(const char* assets_path)
{
	std::string meta_file = assets_path;
	meta_file.append(".meta");
	return meta_file;
}

void ModuleResources::AddFileExtension(std::string & file, ResourceType type)
{
	switch (type)
	{
	case RESOURCE_MODEL: file += ".modelr"; break;
	case RESOURCE_MESH: file += ".meshr"; break;
	case RESOURCE_MATERIAL: file += ".materialr"; break;
	case RESOURCE_TEXTURE: file += ".texturer"; break;
	case RESOURCE_SCENE:  file += ".scener";	break;
	default: break;
	}
}

void ModuleResources::CheckAssetsRecursive(const char* directory)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	App->fileManager->DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		CheckAssetsRecursive((dir + (*it)).c_str());
	}

	std::sort(files.begin(), files.end());

	for (size_t i = 0; i < files.size(); i++)
	{
		std::string file = directory;
		file.append("/" + files[i]);

		//ignore metas and jsons
		if ((file.find(".meta") != std::string::npos) || (file.find(".json") != std::string::npos))
			continue;

		std::string meta = file;
		meta.append(".meta");

		if (App->fileManager->Exists(meta.c_str()))
		{
			if (!MetaUpToDate(file.c_str(), meta.c_str()))
			{
				ReimportFile(file.c_str());
			}
		}
		else
		{
			ImportFile(file.c_str());
		}
	}
}