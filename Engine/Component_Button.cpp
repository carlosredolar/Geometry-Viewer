#include "Component_Button.h"
#include "Component_Canvas.h"
#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "ResourceTexture.h"
#include "GuiAssets.h"
#include "Libs/ImGui/imgui.h"

Component_Button::Component_Button(GameObject* parent) : Component_Graphic(ComponentType::BUTTON, parent)
{
	unpressed = dynamic_cast<ResourceTexture*>(App->resources->LoadResource(App->resources->Find("Assets/EngineAssets/button_default.png"), ResourceType::RESOURCE_TEXTURE));
	pressed = unpressed;
	hovered = pressed;
	deactivated = hovered;
	GenerateMesh(unpressed->GetWidth(), unpressed->GetHeight());
}

Component_Button::~Component_Button() {}

void Component_Button::Update()
{
	if (active) 
	{
		if(clicked)
		{
			DrawGraphic(pressed->GetGpuID(), colorPressed);
			if (timer.ReadSec() >= secsOnClick)
			{
				timer.Stop();
				clicked = false;
			}
		}
		else
		{
			if (MouseOver())
			{
				DrawGraphic(hovered->GetGpuID(), colorHovered);
				if(App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
					OnClick();
			}
			else
			{
				DrawGraphic(unpressed->GetGpuID(), colorUnpressed);
			}
		}
	}
	else DrawGraphic(deactivated->GetGpuID(), colorDeactivated);
}

bool Component_Button::MouseOver()
{
	if (App->in_game)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Component_Button::OnClick()
{
	clicked = true;
	timer.Start();

	canvas->StartFade();
}

void Component_Button::OnGUI()
{
	if (ImGui::CollapsingHeader("Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled ", &enabled);

		ImGui::Spacing();

		ImGui::Checkbox("Interactable", &active);

		const char* options[] = { "Images", "Colors" };
		static int item_current_index = 0;
		const char* current_item = options[item_current_index];
		if (ImGui::BeginCombo("Change type", current_item))
		{
			for (int n = 0; n < IM_ARRAYSIZE(options); n++)
			{
				const bool is_selected = (item_current_index == n);
				if (ImGui::Selectable(options[n], is_selected))
					item_current_index = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (unpressed != nullptr)
		{
			ImGui::Text("Unpressed button");
			vec2 imageSize = normalize(vec2(unpressed->GetWidth(), unpressed->GetHeight()))*100;
			ImGui::Image((ImTextureID)unpressed->GetGpuID(), ImVec2(imageSize.x, imageSize.y), ImVec2(0, 1), ImVec2(1, 0));
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

		//Color unpressed
		ImVec4 buttonColor = { colorUnpressed.r, colorUnpressed.g, colorUnpressed.b, colorUnpressed.a };
		if (ImGui::ColorButton("Color Unpressed", buttonColor, 0, ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize("Color Unpressed ").x, 20)))
			ImGui::OpenPopup("unpressedColPicker");

		if (ImGui::BeginPopup("unpressedColPicker"))
		{
			ImGui::ColorPicker4("##picker", &colorUnpressed, ImGuiColorEditFlags_None, NULL);
			if (ImGui::Button("Close", ImVec2(ImGui::GetWindowContentRegionWidth(), 20))) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		

		if (item_current_index == 0)
		{
			ImGui::Text("Color ");
			if (pressed != nullptr)
			{
				ImGui::Text("Pressed button");
				vec2 imageSize = normalize(vec2(unpressed->GetWidth(), unpressed->GetHeight())) * 100;
				ImGui::Image((ImTextureID)pressed->GetGpuID(), ImVec2(imageSize.x, imageSize.y), ImVec2(0, 1), ImVec2(1, 0));
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
				vec2 imageSize = normalize(vec2(unpressed->GetWidth(), unpressed->GetHeight())) * 100;
				ImGui::Image((ImTextureID)hovered->GetGpuID(), ImVec2(imageSize.x, imageSize.y), ImVec2(0, 1), ImVec2(1, 0));
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
				vec2 imageSize = normalize(vec2(unpressed->GetWidth(), unpressed->GetHeight())) * 100;
				ImGui::Image((ImTextureID)deactivated->GetGpuID(), ImVec2(imageSize.x, imageSize.y), ImVec2(0, 1), ImVec2(1, 0));
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
		else
		{
			ImGui::Text("Color Unpressed");
			//Color pressed
			ImVec4 buttonColor1 = { colorPressed.r, colorPressed.g, colorPressed.b, colorPressed.a };
			if (ImGui::ColorButton("Color Pressed", buttonColor1, 0, ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize("Color Pressed ").x, 20)))
				ImGui::OpenPopup("pressedColPicker");

			if (ImGui::BeginPopup("pressedColPicker"))
			{
				ImGui::ColorPicker4("##picker", &colorPressed, ImGuiColorEditFlags_None, NULL);
				if (ImGui::Button("Close", ImVec2(ImGui::GetWindowContentRegionWidth(), 20))) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::Text("Color Pressed");

			//Color hovered
			ImVec4 buttonColor2 = { colorHovered.r, colorHovered.g, colorHovered.b, colorHovered.a };
			if (ImGui::ColorButton("Color Hovered", buttonColor2, 0, ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize("Color Hovered ").x, 20)))
				ImGui::OpenPopup("hoveredColPicker");

			if (ImGui::BeginPopup("hoveredColPicker"))
			{
				ImGui::ColorPicker4("##picker", &colorHovered, ImGuiColorEditFlags_None, NULL);
				if (ImGui::Button("Close", ImVec2(ImGui::GetWindowContentRegionWidth(), 20))) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::Text("Color Hovered");

			//Color deactivated
			ImVec4 buttonColor3 = { colorDeactivated.r, colorDeactivated.g, colorDeactivated.b, colorDeactivated.a };
			if (ImGui::ColorButton("Color Deactivated", buttonColor3, 0, ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize("Color Deactivated ").x, 20)))
				ImGui::OpenPopup("deactivatedColPicker");

			if (ImGui::BeginPopup("deactivatedColPicker"))
			{
				ImGui::ColorPicker4("##picker", &colorDeactivated, ImGuiColorEditFlags_None, NULL);
				if (ImGui::Button("Close", ImVec2(ImGui::GetWindowContentRegionWidth(), 20))) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::Text("Color Deactivated");
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
	saveObject.AddBool("Active", active);
	saveObject.AddBool("Clicked", clicked);
	if (unpressed != nullptr) saveObject.AddInt("Unpressed Image UID", unpressed->GetUID());
	if (pressed != nullptr) saveObject.AddInt("Pressed Image UID", pressed->GetUID());
	if (hovered != nullptr) saveObject.AddInt("Hovered Image UID", hovered->GetUID());
	if (deactivated != nullptr) saveObject.AddInt("Deactivated Image UID", deactivated->GetUID());
	saveObject.AddColor("Color Unpressed", colorUnpressed);
	saveObject.AddColor("Color Pressed", colorPressed);
	saveObject.AddColor("Color Hovered", colorHovered);
	saveObject.AddColor("Color Deactivated", colorDeactivated);

	saveArray.AddObject(saveObject);
}

void Component_Button::Load(JsonObj & loadObject)
{
	active = loadObject.GetBool("Active");
	clicked = loadObject.GetBool("Clicked");
	int unpressedUID = loadObject.GetInt("Unpressed Image UID");
	if (unpressedUID != -1)
	{
		unpressed = (ResourceTexture*)App->resources->RequestResource(unpressedUID);
		GenerateMesh(unpressed->GetWidth(), unpressed->GetHeight());
	}
	int pressedUID = loadObject.GetInt("Pressed Image UID");
	if (pressedUID != -1)
	{
		pressed = (ResourceTexture*)App->resources->RequestResource(pressedUID);
		GenerateMesh(pressed->GetWidth(), pressed->GetHeight());
	}
	int hoveredUID = loadObject.GetInt("Hovered Image UID");
	if (hoveredUID != -1)
	{
		hovered = (ResourceTexture*)App->resources->RequestResource(hoveredUID);
		GenerateMesh(hovered->GetWidth(), hovered->GetHeight());
	}
	int deactivatedUID = loadObject.GetInt("Deactivated Image UID");
	if (deactivatedUID != -1)
	{
		deactivated = (ResourceTexture*)App->resources->RequestResource(deactivatedUID);
		GenerateMesh(deactivated->GetWidth(), deactivated->GetHeight());
	}
	colorUnpressed = loadObject.GetColor("Color Unpressed");
	colorPressed = loadObject.GetColor("Color Pressed");
	colorHovered = loadObject.GetColor("Color Hovered");
	colorDeactivated = loadObject.GetColor("Color Deactivated");

	canvas->AddElement(this);
}