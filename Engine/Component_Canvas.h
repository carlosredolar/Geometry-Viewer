#ifndef __CANVAS_H__
#define __CANVAS_H__
#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

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

};

#endif //__CANVAS_H__