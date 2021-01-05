#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H

#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Transform;
class ResourceTexture;
class Component_CanvasRenderer;
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
	void DrawGraphic(ResourceTexture* texture);
	void DrawTranforms();

	void SetNullCanvas();

	Component_CanvasRenderer* GetCanvasRenderer()const;
	bool GetParentActive();
	//virtual int GetWidth()const;
	//virtual int GetHeight()const;

	void ResizeGenerateMesh();

protected:
	void GenerateMesh();
protected:
	Component_Canvas* canvas = nullptr;
	Component_CanvasRenderer* canvasRenderer = nullptr;
};

#endif//COMPONENT_GRAPHIC_H