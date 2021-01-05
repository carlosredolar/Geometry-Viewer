#include "Component_Graphic.h"
#include "Component_Transform.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Application.h"

Component_Graphic::Component_Graphic(ComponentType type, GameObject* parent, Component_Canvas* canvas, Component_CanvasRenderer renderer) : Component(type, parent)
{
	if (canvasRenderer == nullptr)
	{
		canvasRenderer = (Component_CanvasRenderer*)ownerGameObject->AddComponent(CANVASRENDERER);
	}
}

Component_Graphic::~Component_Graphic() 
{
	canvas = nullptr;
	canvasRenderer = nullptr;
}

void Component_Graphic::Save(JsonArray& saveArray) {}

void Component_Graphic::Load(JsonObj& loadObject) {}

void Component_Graphic::AddCanvas() {} // ONLY IF THERE IS NO CANVAS IN THE SCENE
void Component_Graphic::AddCanvasRender() {} // ALWAYS ADDED WHEN THIS COMPONENT IS CREATED
void Component_Graphic::SyncComponent(GameObject* sync_parent) {}
void Component_Graphic::DrawGraphic(uint texture, Color color) 
{
	//DRAW THE CORRESPONDING GRAPHIC
	if (canvasRenderer != nullptr)
	{
		canvasRenderer->DrawGraphic(texture, color);
	}
}
void Component_Graphic::DrawTranforms() {} //DRAW THE LIMITS OF THE RECT TRANSFORM (ONLY IN EDIT MODE)

void Component_Graphic::SetNullCanvas() {} //ONLY IF CANVAS IS DELETED AND THIS OBJECT NOT

Component_CanvasRenderer* Component_Graphic::GetCanvasRenderer()const
{
	return canvasRenderer;
}
bool Component_Graphic::GetParentActive() { return false; }

void Component_Graphic::GenerateMesh(int width, int height)
{
	//Generate the quad that will hold the texture of the component
	//Save it in canvasrenderer component
	canvasRenderer->GetVertices()[0] = 0.0f;
	canvasRenderer->GetVertices()[1] = 0.0f;
	canvasRenderer->GetVertices()[2] = width;
	canvasRenderer->GetVertices()[3] = 0.0f;
	canvasRenderer->GetVertices()[4] = width;
	canvasRenderer->GetVertices()[5] = height;
	canvasRenderer->GetVertices()[6] = 0.0f;
	canvasRenderer->GetVertices()[7] = height;
}

void Component_Graphic::ResizeGenerateMesh()
{

}