#include "ResourceModel.h"
#include "Application.h"
#include "ModuleJson.h"

#include "Component_Camera.h"
#include "Component_Light.h"

#include "ImportingSettings.h"

ResourceModel::ResourceModel(uint UID) : Resource(UID, ResourceType::RESOURCE_MODEL) 
{}

ResourceModel::~ResourceModel()
{
	for (size_t i = 0; i < cameras.size(); i++)
	{
		cameras[i] = nullptr;
	}

	for (size_t i = 0; i < lights.size(); i++)
	{
		lights[i] = nullptr;
	}

	nodes.clear();
	meshes.clear();
	materials.clear();
	textures.clear();
	cameras.clear();
	lights.clear();
}

uint ResourceModel::Save(JsonObj& savingObj)
{
	return 1;
}

uint ResourceModel::SaveMeta(JsonObj& savingObj, uint lastMod)
{
	savingObj.AddInt("UID", _uid);
	savingObj.AddString("Library path", App->resources->GetLibraryPath(_uid));
	savingObj.AddInt("lastModified", lastMod);


	ModelImportSettings importSettings = App->resources->modelImportSettings;


	savingObj.AddFloat("global scale", (float)importSettings.globalScale);
	savingObj.AddInt("forward axis", (int)importSettings.forwardAxis);
	savingObj.AddInt("up axis", (int)importSettings.upAxis);
	savingObj.AddBool("ignore cameras", importSettings.ignoreCameras);
	savingObj.AddBool("ignore lights", importSettings.ignoreLights);

	JsonArray nodes_array = savingObj.AddArray("Nodes");

	for (size_t i = 0; i < nodes.size(); i++)
	{
		JsonObj node_object;

		node_object.AddString("Name", nodes[i].name.c_str());
		node_object.AddInt("UID", nodes[i].UID);
		node_object.AddInt("Parent UID", nodes[i].parentUID);
		
		node_object.AddFloat3("Position", nodes[i].position);
		node_object.AddQuaternion("Rotation", nodes[i].rotation);
		node_object.AddFloat3("Scale", nodes[i].scale);

		if (nodes[i].meshID != -1)
		{
			node_object.AddInt("MeshID", nodes[i].meshID);
			node_object.AddString("mesh_library_path", App->resources->GetLibraryPath(nodes[i].meshID));
		}

		if (nodes[i].materialID != -1)
		{
			node_object.AddInt("MaterialID", nodes[i].materialID);
			node_object.AddString("material_library_path", App->resources->GetLibraryPath(nodes[i].materialID));
		}

		nodes_array.AddObject(node_object);
	}

	JsonArray lights_array = savingObj.AddArray("Lights");
	for (size_t i = 0; i < lights.size(); i++)
	{
		lights[i]->Save(lights_array);
	}

	JsonArray cameras_array = savingObj.AddArray("Cameras");
	for (size_t i = 0; i < cameras.size(); i++)
	{
		cameras[i]->Save(cameras_array);
	}

	return 1;
}
