#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "Component_Graphic.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Color.h"
#include "Timer.h"

class JsonArray;
class ResourceTexture;

class Component_Button : public Component_Graphic {

public:
	Component_Button(GameObject* parent);
	~Component_Button();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void SetUnpressed(ResourceTexture* texture);
	void SetPressed(ResourceTexture* texture);
	void SetHovered(ResourceTexture* texture);
	void SetDeactivated(ResourceTexture* texture);

	bool MouseOver();
	void OnClick();
private:
	bool active = true;
	bool clicked = false;

	Timer timer;
	float secsOnClick = 0.2f;

	ResourceTexture* unpressed = nullptr;
	ResourceTexture* pressed = nullptr;
	ResourceTexture* hovered = nullptr;
	ResourceTexture* deactivated = nullptr;
	Color colorUnpressed = White;
	Color colorPressed = White;
	Color colorHovered = White;
	Color colorDeactivated = White;
};

#endif //__BUTTON_H__
