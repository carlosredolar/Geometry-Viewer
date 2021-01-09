#include "Component_Canvas.h"
#include "GameObject.h"
#include "Component_Transform.h"
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
}

void Component_Canvas::Load(JsonObj& loadObject) 
{
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