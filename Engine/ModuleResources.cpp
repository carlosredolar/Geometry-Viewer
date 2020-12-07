#include "ModuleResources.h"
#include "GameObject.h"
#include "FileManager.h"
#include "ModuleImport.h"
#include "ModuleScene.h"
#include "Application.h"

#include "ModuleJson.h"

#include "Resource.h"
//#include "ResourceModel.h"
//#include "ResourceMesh.h"
//#include "ResourceMaterial.h"
//#include "ResourceTexture.h"

#include "MathGeoLib/include/MathGeoLib.h"

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	//name = "resources";
}

ModuleResources::~ModuleResources() {}

bool ModuleResources::Init()
{
	bool ret = true;

	//MeshImporter::Init();
	//TextureImporter::Init();

	return ret;
}

void ModuleResources::OnEditor()
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

int ModuleResources::MetaUpToDate(const char* asset_path)
{
	std::string meta_file = asset_path;
	meta_file += ".meta";

	if (App->fm->Exists(meta_file.c_str()))
	{
		char* buffer = nullptr;
		uint size = App->fm->Load(meta_file.c_str(), &buffer);
		JsonObj meta(buffer);

		uint UID = meta.GetInt("UID");
		int lastModifiedMeta = meta.GetInt("lastModified");
		uint lastModified = App->fm->GetLastModTime(asset_path);

		if (lastModifiedMeta != lastModified)
			return UpdateMetaFile(asset_path);
		else
			return UID;

		meta.Release();
		RELEASE_ARRAY(buffer);
	}
	else
	{
		return -1;
	}
}

int ModuleResources::UpdateMetaFile(const char* assets_file)
{
	char* buffer = nullptr;
	std::string meta_file = assets_file;
	meta_file.append(".meta");

	uint size = App->fm->Load(meta_file.c_str(), &buffer);
	JsonObj meta(buffer);

	uint UID = meta.GetInt("UID");
	int lastModifiedMeta = meta.GetInt("lastModified");
	//uint lastModified = App->fm->GetLastModTime(asset_path);

	//Find(UID);
	return 0;
}

uint ModuleResources::Find(const char* assets_file)
{
	int UID = MetaUpToDate(assets_file);

	if (UID != -1)
	{
		std::map<uint, Resource*>::iterator it = resources.find(UID);

		if (it == resources.end() || it->second == nullptr)
		{
			std::string library_path = GetLibraryFolder(assets_file);
			library_path += std::to_string(UID);
			AddFileExtension(library_path, GetResourceTypeFromPath(assets_file));

			resources_data[UID].assetsFile = assets_file;
			resources_data[UID].libraryFile = library_path;
			resources_data[UID].type = GetResourceTypeFromPath(assets_file);
		}
	}

	return UID;
}

const char* ModuleResources::Find(uint UID)
{
	std::vector<std::string> directories = { "Library/Config/","Library/Models/","Library/Meshes/","Library/Materials/","Library/Textures/", "Library/Scenes/" };
	std::vector<std::string> extensions = { ".json",".model",".mesh",".material",".dds", ".scene" };

	for (size_t i = 0; i < directories.size(); i++)
	{
		std::string file = directories[i];
		file += std::to_string(UID);
		file += extensions[i];
		if (App->fm->Exists(file.c_str()))
		{
			char* final_file = new char[128];
			strcpy(final_file, file.c_str());
			return final_file;
		}
	}
	return nullptr;
}



uint ModuleResources::ImportFile(const char* assets_file)
{
	int meta_data = MetaUpToDate(assets_file);

	if (meta_data != -1 && meta_data != 0)
		return meta_data;

	ResourceType type = GetResourceTypeFromPath(assets_file);

	Resource* resource = CreateResource(assets_file, type);
	uint ret = 0;

	char* fileBuffer;
	uint size = App->fm->Load(assets_file, &fileBuffer);

	switch (type)
	{
	case RESOURCE_MODEL:
		//ModelImporter::Import(fileBuffer, (ResourceModel*)resource, size); break;
	case RESOURCE_TEXTURE:
		//TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size); break;
	case RESOURCE_SCENE: break;
	default: break;
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
}

uint ModuleResources::ImportInternalResource(const char* path, const void* data, ResourceType type)
{
	uint ret = 0;
	Resource* resource = CreateResource(path, type);

	switch (type)
	{
	case RESOURCE_MESH:
		//MeshImporter::Import((aiMesh*)data, (ResourceMesh*)resource);
		break;
	case RESOURCE_MATERIAL:
		//MaterialImporter::Import((aiMaterial*)data, (ResourceMaterial*)resource);
		break;
	default:
		break;
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);

	return ret;
}

void ModuleResources::CreateResourceData(uint UID, ResourceType type, const char* assets_path)
{
	resources_data[UID].type = type;
	resources_data[UID].assetsFile = assets_path;
	resources_data[UID].libraryFile = GenerateLibraryPath(&Resource(UID, type));

}

Resource* ModuleResources::LoadResource(uint UID)
{
	Resource* resource = CreateResource(UID);

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		//ModelImporter::Load(resource->libraryFile.c_str(), (ResourceModel*)resource);
		break;
	case RESOURCE_MESH:
		//MeshImporter::Load(resource->libraryFile.c_str(), (ResourceMesh*)resource);
		break;
	case RESOURCE_MATERIAL:
		//MaterialImporter::Load(resource->libraryFile.c_str(), (ResourceMaterial*)resource);
		break;
	case RESOURCE_TEXTURE:
		//TextureImporter::Load(resource->libraryFile.c_str(), (ResourceTexture*)resource);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	return resources[UID];
}

Resource* ModuleResources::CreateResource(const char* assetsPath, ResourceType type)
{
	Resource* resource = nullptr;
	uint UID = GenerateUID();

	char* buffer;
	uint size = App->fm->Load(assetsPath, &buffer);

	switch (type)
	{
	case RESOURCE_MODEL:
		//resource = new ResourceModel(UID);
		break;
	case RESOURCE_MESH:
		//resource = new ResourceMesh(UID);
		break;
	case RESOURCE_MATERIAL:
		//resource = new ResourceMaterial(UID);
		break;
	case RESOURCE_TEXTURE:
		//resource = new ResourceTexture(UID);
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
		resource->assetsFile = assetsPath;
		resource->libraryFile = GenerateLibraryPath(resource);

		resources_data[UID].assetsFile = resource->assetsFile;
		resources_data[UID].libraryFile = resource->libraryFile;
	}

	RELEASE_ARRAY(buffer);
	return resource;
}

Resource* ModuleResources::CreateResource(uint UID)
{
	Resource* resource = nullptr;

	//TODO: find file if data does not exist
	ResourceType type = resources_data[UID].type;

	switch (type)
	{
	case RESOURCE_MODEL:
		//resource = new ResourceModel(UID);
		break;
	case RESOURCE_MESH:
		//resource = new ResourceMesh(UID);
		break;
	case RESOURCE_MATERIAL:
		//resource = new ResourceMaterial(UID);
		break;
	case RESOURCE_TEXTURE:
		//resource = new ResourceTexture(UID);
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
		resource->assetsFile = resources_data[UID].assetsFile;
		resource->libraryFile = resources_data[UID].libraryFile;
	}

	return resource;
}

Resource* ModuleResources::RequestResource(uint UID)
{
	Resource* resource = nullptr;
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end() && it->second != nullptr) {
		it->second->referenceCount++;
		return it->second;
	}

	return LoadResource(UID);
}

GameObject* ModuleResources::RequestGameObject(const char* assets_file)
{
	/*ResourceModel* model = (ResourceModel*)RequestResource(Find(assets_file));
	return ModelImporter::Load(resources[model->GetUID()]->libraryFile.c_str(), model);*/
}

void ModuleResources::ReleaseResource(uint UID)
{
	delete resources[UID];
	resources[UID] = nullptr;

	//resources.erase(UID);
}

bool ModuleResources::SaveResource(Resource* resource)
{
	bool ret = true;

	char* buffer;
	uint size = 0;

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		//size = ModelImporter::Save((ResourceModel*)resource, &buffer);
		break;
	case RESOURCE_MESH:
		//size = MeshImporter::Save((ResourceMesh*)resource, &buffer);
		break;
	case RESOURCE_MATERIAL:
		//size = MaterialImporter::Save((ResourceMaterial*)resource, &buffer);
		break;
	case RESOURCE_TEXTURE:
		//size = TextureImporter::Save((ResourceTexture*)resource, &buffer);
		break;
	case RESOURCE_SCENE:
		break;
	default:
		break;
	}

	if (size > 0)
	{
		App->fm->Save(resource->libraryFile.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);
	}

	if (resource->GetType() != ResourceType::RESOURCE_MESH && resource->GetType() != ResourceType::RESOURCE_MATERIAL)
		ret = SaveMetaFile(resource);

	return ret;
}

bool ModuleResources::SaveMetaFile(Resource* resource)
{
	JsonObj base_object;
	resource->SaveMeta(base_object, App->fm->GetLastModTime(resource->assetsFile.c_str()));

	char* meta_buffer = NULL;
	uint meta_size = base_object.Save(&meta_buffer);

	std::string meta_file_name = resource->assetsFile + ".meta";
	App->fm->Save(meta_file_name.c_str(), meta_buffer, meta_size);

	base_object.Release();
	RELEASE_ARRAY(meta_buffer);

	return true;
}

ResourceType ModuleResources::GetResourceTypeFromPath(const char* path)
{

	std::string extension;
	App->fm->SplitFilePath(path, nullptr, nullptr, &extension);

	if (extension == ".fbx") { return ResourceType::RESOURCE_MODEL; }
	else if (extension == ".meshr") { return ResourceType::RESOURCE_MESH; }
	else if (extension == ".png") { return ResourceType::RESOURCE_TEXTURE; }
}

uint ModuleResources::GenerateUID()
{
	return LCG().Int();
}

const char* ModuleResources::GenerateLibraryPath(Resource* resource)
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

std::string ModuleResources::GetLibraryFolder(const char* file_in_assets)
{
	ResourceType type = GetResourceTypeFromPath(file_in_assets);

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
	ResourceType type = GetResourceTypeFromPath(path);
	std::string file;
	std::string extension;
	App->fm->SplitFilePath(path, nullptr, &file, &extension);
	file += extension;

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

void ModuleResources::AddFileExtension(std::string& file, ResourceType type)
{
	switch (type)
	{
	case RESOURCE_MODEL: file += ".modelr"; break;
	case RESOURCE_MESH: file += ".meshr"; break;
	case RESOURCE_MATERIAL: file += ".materialr"; break;
	case RESOURCE_TEXTURE: file += ".texturer"; break;
	case RESOURCE_SCENE:  file += ".scener"; break;
	default: break;
	}
}