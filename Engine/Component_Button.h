#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Color.h"

class JsonArray;
class ResourceTexture;

class Component_Button : public Component {

public:
	Component_Button();
	~Component_Button();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void SetUnpressed(ResourceTexture* texture);
	void SetPressed(ResourceTexture* texture);
	void SetHovered(ResourceTexture* texture);
	void SetDeactivated(ResourceTexture* texture);
private:
	ResourceTexture* unpressed;
	ResourceTexture* pressed;
	ResourceTexture* hovered;
	ResourceTexture* deactivated;
	Color colorUnpressed;
	Color colorPressed;
	Color colorHovered;
	Color colorDeactivated;
};

#endif //__BUTTON_H__
