#pragma once
#include "Globals.h"
#include "Component.h"
#include <vector>;
#include "Component_Material.h"

#include "MathGeoLib/include/MathGeoLib.h"

class ResourceMesh;

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

class Component_Mesh : public Component {
public:
	Component_Mesh();
	virtual ~Component_Mesh();

	void Save(JsonArray& save_array) override;
	void Load(JsonObj& load_object) override;
	void SetResourceUID(uint UID) override;
	Resource* GetResource(ResourceType type) override;

	virtual void Update() override;
	virtual void Render();
	virtual void OnGUI() override;

	void DrawVertexNormals();
	void DrawFaceNormals();

	void GenerateAABB();
	AABB GetAABB();

public:
	const char* name;
	char* path;

	AABB aabb;

private:
	bool drawVertexNormals;
	bool drawFaceFormals;

	ResourceMesh* meshResource;
	
};

class PrimitiveGrid {
public:
	PrimitiveGrid(int size);
	~PrimitiveGrid();

	void Render();

private:
	int size;
};

