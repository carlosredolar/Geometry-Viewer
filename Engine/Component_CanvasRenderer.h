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

struct CanvasVertex
{
	float3 position;
	float2 tex_coords;
};
class Component_CanvasRenderer :public Component
{
public:
	Component_CanvasRenderer(GameObject* parent);
	~Component_CanvasRenderer();
	void Update(float dt);

	void OnGUI();
	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	//void SyncComponent(GameObject* sync_parent);

	void DrawGraphic(ResourceTexture* texture);

	void SetGraphic(Component_Graphic* set_graphic);
	//void SetVertex(data);

private:
	Component_Graphic* graphic = nullptr;

	uint verticesBuffer = 0;
	uint amountVertices = 0;
	float* vertices = nullptr;

	uint indicesBuffer = 0;
	uint amountIndices = 0;
	uint* indices = nullptr;
};

#endif // !COMPONENT_CANVAS_RENDER_H