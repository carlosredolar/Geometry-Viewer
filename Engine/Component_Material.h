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
	void SetResourceUID(uint UID) override;

	void BindTexture();

	void Save(JsonArray& save_array) override;
	void Load(JsonObj& load_object) override;
	void OnGUI() override;

	void SetTexture(ResourceTexture* texture);
	void AssignCheckersImage();
	ResourceTexture* GetDiffuseTexture();

private:
	bool checkersImageActive;
	bool colored;

	ResourceMaterial* _resource;
	ResourceTexture* _diffuseTexture;
	uint checkersID;
};