#ifndef __CANVAS_H__
#define __CANVAS_H__
#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include <vector>
#include "Component_Graphic.h"

class JsonArray;

class Component_Canvas : public Component {

public:
	Component_Canvas(GameObject* parent);
	~Component_Canvas();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void Resize(int width, int height);

private:
	std::vector<Component_Graphic*> graphic_elements;

};

#endif //__CANVAS_H__