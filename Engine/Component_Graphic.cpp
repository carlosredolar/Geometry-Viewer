#include "Component_Graphic.h"
#include "Component_Transform.h"
#include "Component_Canvas.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Application.h"

Component_Graphic::Component_Graphic(ComponentType type, GameObject* parent) : Component(type, parent)
{
	GameObject* goCanvas = App->scene->FindGameObjectWithName("Canvas");
	if (goCanvas != nullptr) canvas = goCanvas->GetComponent<Component_Canvas>();

	if (canvas == nullptr)
	{
		//Create canvas
		Component_Transform* trans = new Component_Transform(true);
		GameObject* can = new GameObject(trans, "Canvas");
		canvas = (Component_Canvas*)can->AddComponent(ComponentType::CANVAS);
		canvas->AddElement(this);
		ownerGameObject->ChangeParent(can);
		App->scene->AddGameObject(can);
	}

	canvasRenderer = ownerGameObject->GetComponent<Component_CanvasRenderer>();

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
	if (canvas != nullptr && canvas->IsEnabled())
	{
		if (canvasRenderer != nullptr)
		{
			canvasRenderer->DrawGraphic(texture, color);
		}
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

	position = ownerGameObject->GetTransform()->GetPosition();
}
