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
	strncpy(buf, selected->GetName(), 128);
	transformGO = selected->GetComponent<Component_Transform>();
	meshGO = selected->GetComponent<Component_Mesh>();
	textureGO = selected->GetComponent<Component_Material>();
}

void GuiInspector::Draw()
{
	Begin("Inspector", &is_on);
	if (gameObjectSelected != nullptr) {
		bool gameobjectCheck = gameObjectSelected->IsEnabled();
		Checkbox("Active", &gameobjectCheck);
		(gameobjectCheck) ? gameObjectSelected->Enable() : gameObjectSelected->Disable();
		SameLine();
		InputText("Name", buf, ARRAYSIZE(buf));
		gameObjectSelected->ChangeName(buf);
		//Old system
		/*
		if (CollapsingHeader("Transform")) 
		{
			float3 tempValues = transformGO->GetPosition();
			float pos[3] = { tempValues.x, tempValues.y, tempValues.z };
			tempValues = transformGO->GetRotationEuler();
			float rot[3] = { tempValues.x, tempValues.y, tempValues.z };
			tempValues = transformGO->GetScale();
			float scl[3] = { tempValues.x, tempValues.y, tempValues.z };
			DragFloat3("Position", pos, 0.05f);
			DragFloat3("Rotation", rot, 0.05f);
			DragFloat3("Scale", scl, 0.05f);
			float3 pos_ = { pos[0], pos[1], pos[2] };
			float3 rot_ = { rot[0], rot[1], rot[2] };
			float3 scl_ = { scl[0], scl[1], scl[2] };
			transformGO->SetTransform(pos_, rot_, scl_);
		}
		if (meshGO)
		{
			if (CollapsingHeader("Mesh"))
			{
				bool meshCheck = meshGO->IsEnabled();
				Checkbox("Mesh Active", &meshCheck);
				(meshCheck) ? meshGO->Enable() : meshGO->Disable();
				strncpy(buf, meshGO->GetName(), 128);
				InputText("Mesh Name", buf, ARRAYSIZE(buf));
				Text("Vertices: ");
				SameLine();
				TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "%i", meshGO->GetVertices());
				Text("Indices: ");
				SameLine();
				TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "%i", meshGO->GetIndices());
				bool vertexnormalsCheck = meshGO->enableVertexNormals;
				Checkbox("Vertex Normals", &vertexnormalsCheck);
				(vertexnormalsCheck) ? meshGO->enableVertexNormals = true : meshGO->enableVertexNormals = false;
				bool facenormalsCheck = meshGO->enableFaceNormals;
				Checkbox("Face Normals", &facenormalsCheck);
				(facenormalsCheck) ? meshGO->enableFaceNormals=true : meshGO->enableFaceNormals=false;
			}
		}
		if (textureGO)
		{
			if (CollapsingHeader("Texture"))
			{
				bool textureCheck = textureGO->IsEnabled();
				Checkbox("Texture Active", &textureCheck);
				(textureCheck) ? textureGO->Enable() : textureGO->Disable();
				Text("Texture Name: ");
				SameLine();
				TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), textureGO->GetName());
				Text("Texture Path: ");
				SameLine();
				TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), textureGO->GetPath());
				Text("Texture Size: ");
				SameLine();
				TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), textureGO->GetSize());
			}
		}
		*/

		gameObjectSelected->OnGUI();

		if (ImGui::CollapsingHeader("Resources")) {
			App->resources->OnGUI();
		}
	}
	End();
}