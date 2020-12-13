#pragma once
#include "Globals.h"
#include "Component.h"
#include <vector>;
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Component_Material.h"


class ResourceMesh;

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

class Component_Mesh : public Component {
public:
	Component_Mesh();
	virtual ~Component_Mesh();
	virtual void Update() override;
	virtual void Render();
	virtual void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	Resource* GetResource(ResourceType type) override;
	void SetResourceUID(uint UID) override;
	

	void RenderVertexNormals();
	void RenderFaceNormals();

	void GenerateAABB();
	AABB GetAABB();

public:
	const char* name;
	char* path;

	bool enableVertexNormals;
	bool enableFaceNormals;

	AABB aabb;

private:
	ResourceMesh* meshResource;
	
};

class PrimitiveGrid {
public:
	PrimitiveGrid(int gridSize);
	~PrimitiveGrid();

	void Render();

private:
	int size;
};

