#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class GameObject;


enum ComponentType {
	TRANSFORM,
	MESH,
	TEXTURE,
	NONE
};

class Component {

public:

	//Constructor
	Component(ComponentType type, GameObject* ownerGameObject, bool enabled = true);

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

	//Get component type
	ComponentType GetType();

	//Public variables
	GameObject* ownerGameObject;

private:

	bool enabled;

	ComponentType	type;

};

#endif // !__COMPONENT_H_