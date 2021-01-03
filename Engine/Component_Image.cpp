#include "Component_Image.h"
#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "ResourceTexture.h"
#include "Component_Transform.h"
#include "GuiAssets.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Glew/include/glew.h"

Component_Image::Component_Image(GameObject* parent) : Component(ComponentType::IMAGE, parent)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersID);
	glBindTexture(GL_TEXTURE_2D, checkersID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	CheckersTexDefault();
	image = nullptr;
}

Component_Image::~Component_Image() {}

void Component_Image::Update() 
{
	if (checkersImageActive)
	{
		glBindTexture(GL_TEXTURE_2D, checkersID);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2f(0.5f, 0.5f);
		glTexCoord2f(1, 1); glVertex2f(3.0f, 0.5f);
		glTexCoord2f(1, 0); glVertex2f(3.0f, 1.5f);
		glTexCoord2f(0, 0); glVertex2f(0.5f, 1.5f);
		glEnd();
	}
	else
	{
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, image->GetGpuID());
		//glBlitFramebuffer(0, 0, image->GetWidth(), image->GetHeight(),
		//	0, 0, ownerGameObject->GetTransform()->GetScale().x, ownerGameObject->GetTransform()->GetScale().y,
		//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, image->GetGpuID());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2f(0.05f, 0.05f);
		glTexCoord2f(1, 1); glVertex2f(0.3f, 0.05f);
		glTexCoord2f(1, 0); glVertex2f(0.3f, 0.15f);
		glTexCoord2f(0, 0); glVertex2f(0.05f, 0.15f);
		glEnd();
	}
}

void Component_Image::OnGUI()
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled ", &enabled);

		ImGui::Spacing();

		if (image != nullptr && checkersImageActive == false)
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
				CheckersTexDefault();
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
		else
		{
			ImGui::Image((ImTextureID)checkersID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

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
						SetImage(dynamic_cast<ResourceTexture*>(possible_texture));
				}

				ImGui::EndDragDropTarget();
			}
		}
	}
}

void Component_Image::SetResourceUID(uint UID)
{
	resourceUID = UID;
	image = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(UID));
	checkersImageActive = false;
}

ResourceTexture* Component_Image::GetImage()
{
	return image;
}

void Component_Image::SetImage(ResourceTexture* texture)
{
	image = texture;
	checkersImageActive = false;
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

void Component_Image::CheckersTexDefault()
{
	int CHECKERS_WIDTH = 64;
	int CHECKERS_HEIGHT = 64;

	GLubyte checkerImage[64][64][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x4) == 0) ^ (((j & 0x4)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}


	glBindTexture(GL_TEXTURE_2D, checkersID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkersImageActive = true;
}