#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class GameObject;

enum class COMPONENT_TYPE {
	TRANSFORM,
	MESH,
	TEXTURE,
	NONE
};

class Component {

public:

	

	//Constructor
	Component(COMPONENT_TYPE type, GameObject* ownerGameObject, bool enabled = true);

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
	COMPONENT_TYPE GetType();

	//Public variables
	bool enabled;
	GameObject* ownerGameObject;

private:

	COMPONENT_TYPE	type;

};

#endif // !__COMPONENT_H_