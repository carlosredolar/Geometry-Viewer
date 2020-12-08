#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include <string>

class GameObject;
class JsonObj;
class JsonArray;
class Resource;
enum ResourceType;

enum ComponentType {
	NONE,
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	LIGHT
};

class Component {

public:

	//Constructor
	Component(ComponentType type, GameObject* ownerGameObject = nullptr, bool enabled = true);

	//Destructor
	~Component();

	//Update
	virtual void Update();

	//Enable Component
	virtual void Enable();

	//Disable Component
	virtual void Disable();

	//Check if component is enabled
	virtual bool IsEnabled();

	virtual void Save(JsonArray& save_array) {};
	virtual void Load(JsonObj& load_object) {};

	//Get component type
	ComponentType GetType();

	virtual void OnGUI() = 0;

	void SetOwnerGameObject(GameObject* gameObject);
	GameObject* GetOwnerGameObject();
	virtual void SetResourceUID(uint UID);
	virtual Resource* GetResource(ResourceType type) { return nullptr; };

protected:
	bool enabled;
	GameObject* ownerGameObject;
	ComponentType type;

	uint resourceUID;
};

#endif // !__COMPONENT_H_