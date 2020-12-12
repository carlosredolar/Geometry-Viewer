#pragma once
#include "Globals.h"
#include <string>

class GameObject;
class JsonObj;
class JsonArray;
class Resource;
enum ResourceType;

enum ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL, 
	CAMERA, 
	LIGHT
};

class Component {
public: 
	Component(ComponentType type);
	Component(ComponentType type, GameObject* gameObject);
	virtual ~Component();
	virtual void Update();
	virtual void Enable();
	virtual void Disable();

	virtual void Save(JsonArray& save_array) {};
	virtual void Load(JsonObj& load_object) {};

	bool IsEnabled();
	ComponentType GetType();
	virtual void OnGUI() = 0;

	void SetGameObject(GameObject* gameObject);
	GameObject* GetGameObject();
	virtual void SetResourceUID(uint UID);
	virtual Resource* GetResource(ResourceType type) { return nullptr; };

public:
	std::string name;

protected:
	ComponentType type;
	GameObject* ownerGameObject;
	bool enabled;

	uint resourceUID;
};
