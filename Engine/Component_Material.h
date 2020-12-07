#pragma once
#ifndef __Component_Material_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"

struct textureInfo;

class Component_Material : public Component {
public:

	//Constructor
	Component_Material(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Material();

	//Update
	void Update() override;
	void OnEditor() override;

	//Enable Component
	void Enable() override;

	//Disable Component
	void Disable() override;

	//Check if component is enabled
	bool IsEnabled() override;

	void CleanUp();

	uint GetIdTexture();
	void SetIdTexture(uint UUID);
	void SetTexture(textureInfo* tex);

	const char* GetName();
	const char* GetPath();
	const char* GetSize();
	 
private:
	textureInfo* texture;

	std::string sizeTexture;
};

#endif // !__Component_Material_H__