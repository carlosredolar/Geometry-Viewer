#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H

#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Transform;
class Component_CanvasRender;
class Component_Canvas;
class Component_Interactive;

class Component_Graphic : public Component
{
public:
	Component_Graphic(GameObject* parent);
	~Component_Graphic();
	void CleanUp();

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void AddCanvas();
	void AddCanvasRender();
	void AddTransform();
	void SyncComponent(GameObject* sync_parent);
	void DrawGraphic();
	void DrawTranforms();

	void SetNullCanvas();

	uint GetTextureID()const;
	Component_Transform* GetTrasnform()const;
	Component_CanvasRender* GetCanvasRender()const;
	float4 GetColor()const;
	bool GetParentActive();
	float GetAlpha()const;
	virtual int GetWidth()const;
	virtual int GetHeight()const;

	void ResizeGenerateMesh();

protected:
	void GenerateMesh();
protected:
	Component_Canvas* canvas = nullptr;
	Component_CanvasRender* canvasRenderer = nullptr;
	Component_Transform* transform = nullptr;
	uint textureId = 0;
	float4 color = float4::one;
};

#endif//COMPONENT_GRAPHIC_H