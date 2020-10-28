#pragma once
#ifndef __COMPONENT_MESH_H__

#include "Component.h"
#include "GameObject.h"

class Component_Mesh : public Component {
public:

	//Constructor
	Component_Mesh(GameObject* owner, bool active = true);

	//Destructor
	~Component_Mesh();


	//Update
	void Update() override;

	//Enable Component
	void Enable() override;

	//Disable Component
	void Disable() override;

	//Check if component is enabled
	bool IsEnabled() override;

};

#endif // !__COMPONENT_MESH_H__
