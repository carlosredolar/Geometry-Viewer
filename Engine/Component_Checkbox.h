#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__
#include "Component_Graphic.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Color.h"
//#include "Timer.h"

class JsonArray;
class ResourceTexture;

class Component_Checkbox : public Component_Graphic {

public:
	Component_Checkbox(GameObject* parent);
	~Component_Checkbox();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void SetFalseImage(ResourceTexture* texture);
	void SetTrueImage(ResourceTexture* texture);

	bool MouseOver();
	void OnClick();
private:
	bool active = false;
	bool state = false;

	//Timer timer;
	//float secsOnClick = 0.2f;

	ResourceTexture* trueImage = nullptr;
	ResourceTexture* falseImage = nullptr;
	Color color = White;
};

#endif //__CHECKBOX_H__
