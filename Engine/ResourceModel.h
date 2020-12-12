#pragma once
#include "Globals.h"
#include "Resource.h"
#include "ModuleJson.h"
#include <vector>

class Component_Light;
class Component_Camera;

struct ModelNode
{
	std::string name;
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	int meshID = -1;
	int materialID = -1;

	uint UID = 0u;
	uint parentUID = 0u;
};

class ResourceModel : public Resource {
public:
	ResourceModel(uint UID);
	~ResourceModel();

	uint Save(JsonObj& base_object) override;
	uint SaveMeta(JsonObj& base_object, uint last_modification) override;

public:
	std::vector<ModelNode> nodes;
	std::vector<uint> meshes;
	std::vector<uint> materials;
	std::vector<uint> textures;
	std::vector<Component_Light*> lights;
	std::vector<Component_Camera*> cameras;
};