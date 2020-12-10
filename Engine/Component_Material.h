#pragma once
#ifndef __Component_Material_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"

class ResourceMaterial;
class ResourceTexture;
class Component_Mesh;

class JsonArray;

class Component_Material : public Component {
public:

	//Constructor
	Component_Material(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Material();

	//Update
	void Update() override;

	void CleanUp();

	uint GetIdTexture();
	void SetIdTexture(uint UUID);
	void SetResourceUID(uint UID) override;

	void Save(JsonArray& save_array) override;
	void Load(JsonObj& load_object) override;
	void OnGUI() override;

	void BindTexture();

	void SetTexture(ResourceTexture* texture);
	void AssignCheckersImage();
	ResourceTexture* GetDiffuseTexture();

	const char* GetName();
	const char* GetPath();
	const char* GetSize();
	 
private:
	bool checkers_image;
	bool colored;

	ResourceMaterial* _resource = nullptr;
	ResourceTexture* _diffuseTexture = nullptr;
	uint checkersID;
};

#endif // !__Component_Material_H__