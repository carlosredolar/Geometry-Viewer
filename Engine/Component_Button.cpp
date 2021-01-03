#include "Component_Button.h"
#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "ResourceTexture.h"
#include "GuiAssets.h"
#include "Libs/ImGui/imgui.h"

Component_Button::Component_Button() : Component(ComponentType::BUTTON)
{
	unpressed = nullptr;
	pressed = nullptr;
	hovered = nullptr;
	deactivated = nullptr;
}

Component_Button::~Component_Button() {}

void Component_Button::Update()
{

}

void Component_Button::OnClick()
{

}

void Component_Button::OnGUI()
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled ", &enabled);

		ImGui::Spacing();

		if (unpressed != nullptr)
		{
			ImGui::Text("Unpressed button");
			ImGui::Image((ImTextureID)unpressed->GetGpuID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
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
						unpressed = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				unpressed = nullptr;
				//CheckersTexDefault();
			}

			//ImGui::Spacing();
			if (unpressed != nullptr)
			{
				ImGui::Text("Texture Path: %s", unpressed->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", unpressed->GetWidth(), unpressed->GetHeight());
				ImGui::Spacing();
			}
		}

		if (pressed != nullptr)
		{
			ImGui::Text("Pressed button");
			ImGui::Image((ImTextureID)pressed->GetGpuID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
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
						pressed = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				pressed = nullptr;
				//CheckersTexDefault();
			}

			//ImGui::Spacing();
			if (pressed != nullptr)
			{
				ImGui::Text("Texture Path: %s", pressed->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", pressed->GetWidth(), pressed->GetHeight());
				ImGui::Spacing();
			}
		}

		if (hovered != nullptr)
		{
			ImGui::Text("Hovered button");
			ImGui::Image((ImTextureID)hovered->GetGpuID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
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
						hovered = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				hovered = nullptr;
				//CheckersTexDefault();
			}

			//ImGui::Spacing();
			if (hovered != nullptr)
			{
				ImGui::Text("Texture Path: %s", hovered->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", hovered->GetWidth(), hovered->GetHeight());
				ImGui::Spacing();
			}
		}

		if (deactivated != nullptr)
		{
			ImGui::Text("Deactivated button");
			ImGui::Image((ImTextureID)deactivated->GetGpuID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
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
						deactivated = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				deactivated = nullptr;
				//CheckersTexDefault();
			}

			//ImGui::Spacing();
			if (deactivated != nullptr)
			{
				ImGui::Text("Texture Path: %s", deactivated->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", deactivated->GetWidth(), deactivated->GetHeight());
				ImGui::Spacing();
			}
		}
	}
}

void Component_Button::SetUnpressed(ResourceTexture * texture)
{
	unpressed = texture;
}

void Component_Button::SetPressed(ResourceTexture * texture)
{
	pressed = texture;
}

void Component_Button::SetHovered(ResourceTexture* texture)
{
	hovered = texture;
}

void Component_Button::SetDeactivated(ResourceTexture* texture)
{
	deactivated = texture;
}

void Component_Button::Save(JsonArray & saveArray)
{
	JsonObj saveObject;

	saveObject.AddInt("Type", type);

	saveObject.AddInt("Image UID", unpressed->GetUID());

	saveArray.AddObject(saveObject);
}

void Component_Button::Load(JsonObj & loadObject)
{
	int imageUID = loadObject.GetInt("Image UID");
	unpressed = (ResourceTexture*)App->resources->RequestResource(imageUID);
}