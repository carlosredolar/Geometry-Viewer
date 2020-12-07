#pragma once
#ifndef __COMPONENT_MESH_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"


struct meshInfo;

class Component_Mesh : public Component {
public:

	//Constructor
	Component_Mesh(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Mesh();

	void GenerateMesh(meshInfo* newMesh, std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords);

	//Update
	void Update() override;

	//Enable Component
	void Enable() override;

	//Disable Component
	void Disable() override;

	//Check if component is enabled
	bool IsEnabled() override;

	const char* GetName();
	int GetVertices();
	int GetIndices();

	void CreateBuffers();

	void Render();

	void RenderVertexNormals(std::vector<float3> vertices, std::vector<float3> normals);
	void RenderFaceNormals(std::vector<float3> vertices, std::vector<float3> normals);

	void CleanUp();

	bool enableVertexNormals = false;
	bool enableFaceNormals = false;

	void GenerateAABB();
	void RenderAABB();
	AABB GetAABB();
	AABB _AABB;
	OBB _OBB;

private:


	uint idTextureImage;

	
	meshInfo* mesh;

};

#endif // !__COMPONENT_MESH_H__
