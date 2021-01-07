#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H

#include "Component.h"
#include "Component_CanvasRenderer.h"
#include "Color.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Transform;
class ResourceTexture;
class Component_Canvas;
class Component_Interactive;

class Component_Graphic : public Component
{
public:
	Component_Graphic(ComponentType type, GameObject* parent);
	~Component_Graphic();

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void AddCanvas();
	void AddCanvasRender();
	void SyncComponent(GameObject* sync_parent);
	void DrawGraphic(uint texture, Color color);
	void DrawTranforms();

	void SetNullCanvas();

	Component_CanvasRenderer* GetCanvasRenderer()const;
	bool GetParentActive();

protected:
	void GenerateMesh(int width, int height);
protected:
	Component_Canvas* canvas = nullptr;
	Component_CanvasRenderer* canvasRenderer = nullptr;

	float3 position = float3::zero;
};

#endif//COMPONENT_GRAPHIC_H