#include "Component_Image.h"
#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "ResourceTexture.h"
#include "GuiAssets.h"
#include "Libs/ImGui/imgui.h"

Component_Image::Component_Image() : Component(ComponentType::TRANSFORM2D)
{
	image = nullptr;
}

Component_Image::~Component_Image() {}

void Component_Image::Update() 
{

}

void Component_Image::OnGUI()
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled ", &enabled);

		ImGui::Spacing();

		if (image != nullptr)
		{
			ImGui::Image((ImTextureID)image->GetGpuID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("ASSETS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					GuiAssets* assets_window = (GuiAssets*)App->gui->windows[ASSETS_WINDOW];
					const char* file = assets_window->GetFileAt(payload_n);
					Resource* possible_texture = App->resources->RequestResource(App->resources->Find(file));

					if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE)
						image = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				image = nullptr;
				//CheckersTexDefault();
			}

			ImGui::Spacing();
			if (image != nullptr)
			{
				ImGui::Text("Texture Path: %s", image->assetsFile.c_str());
				ImGui::Spacing();
				ImGui::Text("Width: %d Height: %d", image->GetWidth(), image->GetHeight());
				ImGui::Spacing();
			}
		}
	}
}

void Component_Image::SetResourceUID(uint UID)
{
	resourceUID = UID;
	image = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(UID));
}

ResourceTexture* Component_Image::GetImage()
{
	return image;
}

void Component_Image::SetImage(ResourceTexture* texture)
{
	image = texture;
}

void Component_Image::Save(JsonArray& saveArray)
{
	JsonObj saveObject;

	saveObject.AddInt("Type", type);

	saveObject.AddInt("Image UID", image->GetUID());

	saveArray.AddObject(saveObject);
}

void Component_Image::Load(JsonObj& loadObject)
{
	int imageUID = loadObject.GetInt("Image UID");
	image = (ResourceTexture*)App->resources->RequestResource(imageUID);
}