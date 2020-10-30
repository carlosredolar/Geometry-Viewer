#include "Application.h"
#include "GuiInspector.h"
#include "ModuleGui.h"

using namespace ImGui;

GuiInspector::GuiInspector() : GuiWindow()
{
	is_on = true;
	strncpy(buf, "Insert a text", 128);
	gameObjectSelected = nullptr;
}

GuiInspector::~GuiInspector()
{}

bool GuiInspector::Start()
{
	bool ret = true;
	return ret;
}

bool GuiInspector::CleanUp()
{
	bool ret = true;

	return ret;
}

void GuiInspector::Select(GameObject* selected) {
	gameObjectSelected = selected;
	strncpy(buf, gameObjectSelected->GetName(), 128);
	goTransform = gameObjectSelected->GetComponent<Component_Transform>();
}

void GuiInspector::Draw()
{
	Begin("Inspector", &is_on);

	if (gameObjectSelected != nullptr) {
		static bool check = true;
		Checkbox("Active", &check);
		SameLine();
		InputText("Name", buf, ARRAYSIZE(buf));
		gameObjectSelected->ChangeName(buf);

		if (CollapsingHeader("Transform")) 
		{
			float3 tempValues = goTransform->GetPosition();
			static float pos[3] = { tempValues.x, tempValues.y, tempValues.z };
			tempValues = goTransform->GetRotationEuler();
			static float rot[3] = { tempValues.x, tempValues.y, tempValues.z };
			tempValues = goTransform->GetScale();
			static float scl[3] = { tempValues.x, tempValues.y, tempValues.z };
			DragFloat3("Position", pos, 0.05f);
			DragFloat3("Rotation", rot, 0.05f);
			DragFloat3("Scale", scl, 0.05f);
			float3 pos_ = { pos[0], pos[1], pos[2] };
			float3 rot_ = { rot[0], rot[1], rot[2] };
			float3 scl_ = { scl[0], scl[1], scl[2] };
			goTransform->SetTransform(pos_, rot_, scl_);
		}



		Text("Here goes the inspector with all the components");
	}

	End();
}