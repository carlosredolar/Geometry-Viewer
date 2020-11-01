#pragma once
#ifndef __Component_Texture_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"

struct textureInfo;

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
	void SetTexture(textureInfo* tex);

	const char* GetName();
	const char* GetPath();
	const char* GetSize();
	 
private:
	textureInfo* texture;

	std::string sizeTexture;
};

#endif // !__Component_Texture_H__