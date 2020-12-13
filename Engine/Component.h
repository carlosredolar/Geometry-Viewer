#pragma once
#include "Globals.h"
#include <string>

class JsonObj;
class JsonArray;

class GameObject;
class Resource;


enum ResourceType;

enum ComponentType {
	MESH,
	MATERIAL, 
	TRANSFORM,
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

	bool IsEnabled();
	virtual void OnGUI() = 0;

	virtual void Save(JsonArray& saveArray) {};
	virtual void Load(JsonObj& loadObject) {};

	//GameObject
	GameObject* GetGameObject();
	void SetGameObject(GameObject* gameObject);
	
	//Component
	ComponentType GetType();

	//Resource
	virtual Resource* GetResource(ResourceType type);
	virtual void SetResourceUID(uint UID);
	

public:
	std::string name;

	bool enabled;

	GameObject* ownerGameObject;
	ComponentType type;	
	uint resourceUID;
};
