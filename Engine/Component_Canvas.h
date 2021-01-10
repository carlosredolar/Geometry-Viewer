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

	void AddElement(Component_Graphic* element);

private:
	std::vector<Component_Graphic*> graphic_elements;

	bool getScale = false;
	float3 canvasScale;

	float2 canvasSize;
	float2 canvasReferenceResolution;
};

#endif //__CANVAS_H__