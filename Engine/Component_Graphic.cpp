#include "Component_Graphic.h"

Component_Graphic::Component_Graphic(ComponentType type, GameObject* parent) : Component(type, parent)
{

}

Component_Graphic::~Component_Graphic() {}

void Component_Graphic::Save(JsonArray& saveArray) {}

void Component_Graphic::Load(JsonObj& loadObject) {}

void Component_Graphic::AddCanvas() {} // ONLY IF THERE IS NO CANVAS IN THE SCENE
void Component_Graphic::AddCanvasRender() {} // ALWAYS ADDED WHEN THIS COMPONENT IS CREATED
void Component_Graphic::SyncComponent(GameObject* sync_parent) {}
void Component_Graphic::DrawGraphic() {} //DRAW THE CORRESPONDING GRAPHIC
void Component_Graphic::DrawTranforms() {} //DRAW THE LIMITS OF THE RECT TRANSFORM (ONLY IN EDIT MODE)

void Component_Graphic::SetNullCanvas() {} //ONLY IF CANVAS IS DELETED AND THIS OBJECT NOT

Component_CanvasRender* Component_Graphic::GetCanvasRender()const
{
	return canvasRenderer;
}
bool Component_Graphic::GetParentActive() {}

void Component_Graphic::GenerateMesh()
{
	//Generate the quad that will hold the texture of the component
}