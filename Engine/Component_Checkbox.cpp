#include "Component_Checkbox.h"
#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "ResourceTexture.h"
#include "GuiAssets.h"
#include "Libs/ImGui/imgui.h"

Component_Checkbox::Component_Checkbox(GameObject* parent) : Component_Graphic(ComponentType::CHECKBOX, parent)
{
	trueImage = dynamic_cast<ResourceTexture*>(App->resources->LoadResource(App->resources->Find("Assets/EngineAssets/checkbox_true.png"), ResourceType::RESOURCE_TEXTURE));
	falseImage = dynamic_cast<ResourceTexture*>(App->resources->LoadResource(App->resources->Find("Assets/EngineAssets/checkbox_false.png"), ResourceType::RESOURCE_TEXTURE));
	GenerateMesh(trueImage->GetWidth(), trueImage->GetHeight());
}

Component_Checkbox::~Component_Checkbox() {}

void Component_Checkbox::Update()
{
	if (active)
	{
		if (MouseOver())
		{
			if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
				OnClick();
		}

		if (state)
		{
			DrawGraphic(trueImage->GetGpuID(), color);
		}
		else
		{
			DrawGraphic(falseImage->GetGpuID(), color);
		}
	}
}

bool Component_Checkbox::MouseOver()
{
	if (App->in_game)
	{
		//Check if mouse is over button
		return true;
	}
	else
	{
		return false;
	}
}

void Component_Checkbox::OnClick()
{
	state = !state;
	App->renderer3D->SetVSync(!App->renderer3D->vsync);
}

void Component_Checkbox::SetState(bool st)
{
	state = st;
}

void Component_Checkbox::OnGUI()
{
	if (ImGui::CollapsingHeader("Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled ", &enabled);

		ImGui::Spacing();

		ImGui::Checkbox("Interactable", &active);

		if (falseImage != nullptr)
		{
			ImGui::Text("falseImage");
			vec2 imageSize = normalize(vec2(falseImage->GetWidth(), falseImage->GetHeight()))*100;
			ImGui::Image((ImTextureID)falseImage->GetGpuID(), ImVec2(imageSize.x, imageSize.y), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("ASSETS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					GuiAssets * assets_window = (GuiAssets*)App->gui->windows[ASSETS_WINDOW];
					const char* file = assets_window->GetFileAt(payload_n);
					Resource * possible_texture = App->resources->RequestResource(App->resources->Find(file));

					if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE)
						falseImage = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				falseImage = nullptr;
				//CheckersTexDefault();
			}

			//ImGui::Spacing();
			if (falseImage != nullptr)
			{
				ImGui::Text("Texture Path: %s", falseImage->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", falseImage->GetWidth(), falseImage->GetHeight());
				ImGui::Spacing();
			}
		}

		//Color
		ImVec4 buttonColor = { color.r, color.g, color.b, color.a };
		if (ImGui::ColorButton("Color", buttonColor, 0, ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize("Color ").x, 20)))
			ImGui::OpenPopup("colorPicker");

		if (ImGui::BeginPopup("colorPicker"))
		{
			ImGui::ColorPicker4("##picker", &color, ImGuiColorEditFlags_None, NULL);
			if (ImGui::Button("Close", ImVec2(ImGui::GetWindowContentRegionWidth(), 20))) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text("Color ");

		//trueImage

		if (trueImage != nullptr)
		{
			ImGui::Text("trueImage");
			vec2 imageSize = normalize(vec2(trueImage->GetWidth(), trueImage->GetHeight())) * 100;
			ImGui::Image((ImTextureID)trueImage->GetGpuID(), ImVec2(imageSize.x, imageSize.y), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("ASSETS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					GuiAssets * assets_window = (GuiAssets*)App->gui->windows[ASSETS_WINDOW];
					const char* file = assets_window->GetFileAt(payload_n);
					Resource * possible_texture = App->resources->RequestResource(App->resources->Find(file));

					if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE)
						trueImage = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				trueImage = nullptr;
				//CheckersTexDefault();
			}

			//ImGui::Spacing();
			if (trueImage != nullptr)
			{
				ImGui::Text("Texture Path: %s", trueImage->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", trueImage->GetWidth(), trueImage->GetHeight());
				ImGui::Spacing();
			}
		}
	}
}

void Component_Checkbox::SetFalseImage(ResourceTexture * texture)
{
	falseImage = texture;
}

void Component_Checkbox::SetTrueImage(ResourceTexture * texture)
{
	trueImage = texture;
}

void Component_Checkbox::Save(JsonArray & saveArray)
{
	JsonObj saveObject;

	saveObject.AddInt("Type", type);
	saveObject.AddBool("Active", active);
	saveObject.AddBool("State", state);
	if (falseImage != nullptr) saveObject.AddInt("False Image UID", falseImage->GetUID());
	if (trueImage != nullptr) saveObject.AddInt("True Image UID", trueImage->GetUID());
	saveObject.AddColor("Color", color);

	saveArray.AddObject(saveObject);
}

void Component_Checkbox::Load(JsonObj& loadObject)
{
	active = loadObject.GetBool("Active");
	state = loadObject.GetBool("State");
	int trueImageUID = loadObject.GetInt("True Image UID");
	if (trueImageUID != -1)
	{
		trueImage = (ResourceTexture*)App->resources->RequestResource(trueImageUID);
		GenerateMesh(trueImage->GetWidth(), trueImage->GetHeight());
	}
	int falseImageUID = loadObject.GetInt("False Image UID");
	if (falseImageUID != -1)
	{
		falseImage = (ResourceTexture*)App->resources->RequestResource(falseImageUID);
		GenerateMesh(falseImage->GetWidth(), falseImage->GetHeight());
	}
	color = loadObject.GetColor("Color");
}