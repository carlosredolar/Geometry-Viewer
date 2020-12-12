#include "GuiInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

GuiInspector::GuiInspector() : GuiWindow()
{
	type = WindowType::INSPECTOR_WINDOW;
}

GuiInspector::~GuiInspector()
{
}

void GuiInspector::Draw()
{
	if (ImGui::Begin("Inspector", &visible))
	{
		if (App->scene->selectedGameObject != nullptr)
		{
			App->scene->selectedGameObject->OnGUI();
		}
	}
	ImGui::End();
}
