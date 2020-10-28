#pragma once
#ifndef __COMPONENT_MESH_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Component_Mesh : public Component {
public:

	//Constructor
	Component_Mesh(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Mesh();

	void GenerateMesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords);

	//Update
	void Update() override;

	//Enable Component
	void Enable() override;

	//Disable Component
	void Disable() override;

	//Check if component is enabled
	bool IsEnabled() override;

	void CreateBuffers();
	void Render();

	void CleanUp();

private:
	//uint id_index = 0; // index in VRAM
	//uint* index = nullptr;

	//uint id_vertex = 0; // unique vertex in VRAM
	//float* vertex = nullptr;

	uint idVertex;
	std::vector<float3> vertices;

	uint idIndex;
	std::vector<uint> indices;

	uint idNormals;
	std::vector<float3> normals;

	uint idTextureCoords;
	std::vector<float2> textureCoords;

	uint idTextureImage;
};

#endif // !__COMPONENT_MESH_H__
