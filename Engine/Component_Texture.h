#pragma once
#ifndef __Component_Texture_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Component_Texture : public Component {
public:

	//Constructor
	Component_Texture(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Texture();

	//Update
	void Update() override;

	//Enable Component
	void Enable() override;

	//Disable Component
	void Disable() override;

	//Check if component is enabled
	bool IsEnabled() override;

	void CleanUp();

	uint GetIdTexture();
	void SetIdTexture(uint id);

private:
	uint idTextureImage;
};

#endif // !__Component_Texture_H__