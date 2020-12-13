#pragma once
#include "Globals.h"
#include "Resource.h"
#include "ModuleJson.h"

#include <vector>

class Component_Camera;
class Component_Light;

struct ModelNode
{
	std::string name;

	int meshID = -1;
	int materialID = -1;

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	uint UID = 0u;
	uint parentUID = 0u;
};

class ResourceModel : public Resource {
public:
	ResourceModel(uint UID);
	~ResourceModel();

	uint Save(JsonObj& savingObj) override;
	uint SaveMeta(JsonObj& savingObj, uint lastMod) override;

public:
	std::vector<ModelNode> nodes;
	std::vector<uint> meshes;
	std::vector<uint> materials;
	std::vector<uint> textures;
	std::vector<Component_Camera*> cameras;
	std::vector<Component_Light*> lights;
};