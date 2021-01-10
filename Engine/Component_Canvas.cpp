#include "Component_Canvas.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Component_Camera.h"
#include "ModuleJson.h"
#include "Libs/Glew/include/glew.h"
#include "Libs/ImGui/imgui.h"
#include "Application.h"
#include "ModuleGui.h"

Component_Canvas::Component_Canvas(GameObject* parent) : Component(CANVAS, parent)
{
	canvasReferenceResolution = float2(1920, 1080);
	canvasSize = float2(App->window->width, App->window->height);
	//ownerGameObject->GetTransform()->SetSize(canvasSize);
}

Component_Canvas::~Component_Canvas()
{
}

void Component_Canvas::Update() 
{
	if (App->in_game)
	{
		float3 camRotation = App->renderer3D->GetMainCamera()->ownerGameObject->GetTransform()->GetEulerRotation();
		ownerGameObject->GetTransform()->SetRotation(camRotation.x, camRotation.y + 180, camRotation.z);

		float3 cornerPoints[8];
		App->renderer3D->GetMainCamera()->GetFrustum().GetCornerPoints(cornerPoints);

		if (!getScale)
		{
			canvasScale = ownerGameObject->GetTransform()->GetScale();
			getScale = true;
		}
		ownerGameObject->GetTransform()->SetScale(canvasScale.Mul(cornerPoints[1].Distance(cornerPoints[5]) / canvasSize.x));

		ownerGameObject->GetTransform()->SetPosition(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	}
	else getScale = false;

	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushMatrix();
	glMultMatrixf((float*)& ownerGameObject->GetTransform()->GetGlobalTransform().Transposed());

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(canvasSize.x, 0);
	glVertex2f(canvasSize.x, canvasSize.y);
	glVertex2f(0, canvasSize.y);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void Component_Canvas::OnGUI() 
{
	if (ImGui::CollapsingHeader("Canvas", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled", &enabled);
		ImGui::Separator();
		int size[2] = { canvasSize.x, canvasSize.y };
		ImGui::InputInt2("Canvas Size", size, ImGuiInputTextFlags_ReadOnly);
		int sizeRef[2] = { canvasReferenceResolution.x, canvasReferenceResolution.y };
		if(ImGui::DragInt2("Reference Resolution", sizeRef, 1, 0, 10000))
		{
			canvasReferenceResolution.Set(sizeRef[0], sizeRef[1]);
		}
	}
}

void Component_Canvas::Save(JsonArray& saveArray) 
{
	JsonObj saveObject;
	saveObject.AddInt("Type", type);
	saveObject.AddFloat("Canvas Size x", canvasSize.x);
	saveObject.AddFloat("Canvas Size y", canvasSize.y);
	saveObject.AddFloat("Ref Res x", canvasReferenceResolution.x);
	saveObject.AddFloat("Ref Res y", canvasReferenceResolution.y);
	saveArray.AddObject(saveObject);
}

void Component_Canvas::Load(JsonObj& loadObject) 
{
	canvasSize.x = loadObject.GetFloat("Canvas Size x");
	canvasSize.y = loadObject.GetFloat("Canvas Size y");
	canvasReferenceResolution.x = loadObject.GetFloat("Ref Res x");
	canvasReferenceResolution.y = loadObject.GetFloat("Ref Res y");
}

void Component_Canvas::Resize(int width, int height)
{
	//TODO: Resize this transform
	canvasSize = float2(width, height);

	for (uint i = 0; i < graphic_elements.size(); i++)
	{
		//Move objects reference anchor
	}
}

void Component_Canvas::AddElement(Component_Graphic* element)
{
	graphic_elements.push_back(element);
}