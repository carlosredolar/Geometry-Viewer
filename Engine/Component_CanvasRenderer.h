#ifndef COMPONENT_CANVAS_RENDER_H
#define COMPONENT_CANVAS_RENDER_H

#include "Component.h"
#include <vector>
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Graphic;
class Component_Text;
class Component_Image;
class Component_Canvas;
class ResourceTexture;
enum FillMethod;
struct Color;

class Component_CanvasRenderer :public Component
{
public:
	Component_CanvasRenderer(GameObject* parent);
	~Component_CanvasRenderer();

	void OnGUI();
	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	//void SyncComponent(GameObject* sync_parent);

	void DrawGraphic(uint texture, Color color, bool inGame = false);

	float* GetVertices();
	void SetTextureTile(float2 texTile);

	void SetAlpha(float a);

private:
	float* vertices = nullptr;

	float2 textureTile = float2::one;

	float alphaCanvas = 1;
};

#endif // !COMPONENT_CANVAS_RENDER_H