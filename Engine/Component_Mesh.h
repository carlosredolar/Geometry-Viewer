#pragma once
#ifndef __COMPONENT_MESH_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ResourceMesh;

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

class Component_Mesh : public Component {
public:

	//Constructor
	Component_Mesh(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Mesh();

	void Save(JsonArray& save_array) override;
	void Load(JsonObj& load_object) override;
	void SetResourceUID(uint UID) override;
	Resource* GetResource(ResourceType type) override;

	//Update
	void Update() override;
	void Render();
	void OnGUI() override;

	void RenderVertexNormals();
	void RenderFaceNormals();

	void GenerateAABB();
	AABB GetAABB();

	const char* GetName();
	int GetVertices();
	int GetIndices();

public:
	const char* name;
	char* path;

	bool enableVertexNormals = false;
	bool enableFaceNormals = false;
private:
	
	AABB AABB;
	ResourceMesh* mesh;
};

#endif // !__COMPONENT_MESH_H__
