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
	obb = aabb;
	obb.Transform(ownerGameObject->GetTransform()->GetGlobalTransform());
	drawAABB.SetNegativeInfinity();
	drawAABB.Enclose(obb);

	if (App->scene->showBB)
	{
		float3 cornerPoints[8];
		drawAABB.GetCornerPoints(cornerPoints);

		ImVec4 colorBB = ImGui::GetStyleColorVec4(ImGuiCol_Button);

		App->renderer3D->DrawAABB(cornerPoints, colorBB);
	}
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
	textureSize.Set(width, height);
	Component_Transform* transform = ownerGameObject->GetTransform();
	float2 pivotOffset = transform->GetPivot().Mul(textureSize);

	canvasRenderer->GetVertices()[0] = 0.0f - pivotOffset.x;
	canvasRenderer->GetVertices()[1] = 0.0f - pivotOffset.y;
	canvasRenderer->GetVertices()[2] = width - pivotOffset.x;
	canvasRenderer->GetVertices()[3] = 0.0f - pivotOffset.y;
	canvasRenderer->GetVertices()[4] = width - pivotOffset.x;
	canvasRenderer->GetVertices()[5] = height - pivotOffset.y;
	canvasRenderer->GetVertices()[6] = 0.0f - pivotOffset.x;
	canvasRenderer->GetVertices()[7] = height - pivotOffset.y;

	position = transform->GetPosition();

	GenerateAABB();
}

void Component_Graphic::GenerateAABB()
{
	float* verts = new float[12];
	int j = 0;
	for (int i = 0; i < 9; i+=3)
	{
		verts[i] = canvasRenderer->GetVertices()[j];
		verts[i+1] = canvasRenderer->GetVertices()[j+1];
		verts[i + 2] = position.z;
		j += 2;
	}
	aabb.SetNegativeInfinity();
	aabb.Enclose((float3*)verts, 3);

	ownerGameObject->SetAABB(aabb);
}

AABB Component_Graphic::GetAABB()
{
	return drawAABB;
}
