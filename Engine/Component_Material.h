#pragma once
#include "Component.h"
#include "Globals.h"

#include <string>

class Component_Mesh;
class ResourceMaterial;
class ResourceTexture;
class JsonArray;

class Component_Material : public Component {
public: 
	Component_Material();
	Component_Material(GameObject* gameObject);
	~Component_Material();

	void Update() override;
	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;
	void OnGUI() override;

	void BindTexture();

	void SetResourceUID(uint UID) override;

	ResourceTexture* GetDiffuseTexture();
	void SetTexture(ResourceTexture* texture);
	void CheckersTexDefault();	

private:
	bool checkersImageActive;
	bool colored;

	uint checkersID;

	ResourceMaterial* materialResource;
	ResourceTexture* diffuseTexture;
};