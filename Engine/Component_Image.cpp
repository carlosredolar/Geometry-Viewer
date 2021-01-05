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

Component_Image::Component_Image(GameObject* parent) : Component_Graphic(ComponentType::IMAGE, parent)
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

	color.Set(1, 1, 1, 1);

	CheckersTexDefault();
}

Component_Image::~Component_Image() {}

void Component_Image::Update() 
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glMultMatrixf((float*)& ownerGameObject->GetTransform()->GetGlobalTransform().Transposed());
	glColor4f(color.r, color.g, color.b, color.a);
	if (checkersImageActive)
	{
		glBindTexture(GL_TEXTURE_2D, checkersID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, image->GetGpuID());
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0, 1); glVertex2f(0.0f, 1.0f);
	glTexCoord2f(1, 1); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(1, 0); glVertex2f(1.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
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
		float col[4] = { color.r, color.g, color.b, color.a };
		if (ImGui::DragFloat4("Color", col, 0.1f, 0.0f, 1.0f))
		{
			color.Set(col[0], col[1], col[2], col[3]);
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