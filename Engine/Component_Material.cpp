#include "Component_Material.h"
#include "Component_Mesh.h"
#include "ImGui/imgui.h"
#include "FileManager.h"
#include "ModuleJson.h"
#include "GameObject.h"
#include "Application.h"
#include "ResourceTexture.h"
#include "glew/include/glew.h"
#include "ResourceMaterial.h"
#include "GuiAssets.h"

Component_Material::Component_Material() : Component(ComponentType::MATERIAL), checkersImageActive(false), materialResource(nullptr), colored(false), _diffuseTexture(nullptr)
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
}

Component_Material::Component_Material(GameObject* gameObject) : Component(ComponentType::MATERIAL, gameObject), checkersImageActive(false), materialResource(nullptr), colored(false), _diffuseTexture(nullptr)
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
}

Component_Material::~Component_Material()
{
	if (materialResource != nullptr)
	{
		App->resources->ReleaseResource(resourceUID);
		materialResource = nullptr;

		if (_diffuseTexture != nullptr)
		{
			App->resources->ReleaseResource(_diffuseTexture->GetUID());
			_diffuseTexture = nullptr;
		}
	}

	glDeleteTextures(1, &checkersID);
}

void Component_Material::Update() {}

void Component_Material::SetResourceUID(uint UID)
{
	resourceUID = UID;
	materialResource = dynamic_cast<ResourceMaterial*>(App->resources->RequestResource(UID));

	if (materialResource->diffuseTextureUID != 0)
	{
		if (_diffuseTexture != nullptr)
			App->resources->ReleaseResource(_diffuseTexture->GetUID());

		_diffuseTexture = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(materialResource->diffuseTextureUID));
	}

	if (_diffuseTexture == nullptr)
		AssignCheckersImage();
	else
		SetTexture(_diffuseTexture);
}

void Component_Material::BindTexture()
{
	if (!checkersImageActive) _diffuseTexture->BindTexture();
	else glBindTexture(GL_TEXTURE_2D, checkersID);
}

void Component_Material::Save(JsonArray& save_array)
{
	JsonObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddInt("Material UID", materialResource->GetUID());

	if (_diffuseTexture != nullptr)
		save_object.AddInt("Texture UID", _diffuseTexture->GetUID());

	save_array.AddObject(save_object);
}

void Component_Material::Load(JsonObj& load_object)
{
	int materialUID = load_object.GetInt("Material UID");
	materialResource = (ResourceMaterial*)App->resources->RequestResource(materialUID);

	int textureUID = load_object.GetInt("Texture UID", -1);

	if (materialResource != nullptr && textureUID != -1) 
	{
		materialResource->diffuseTextureUID = textureUID;
		_diffuseTexture = (ResourceTexture*)App->resources->RequestResource(textureUID);
	}

	if (_diffuseTexture == nullptr)
		AssignCheckersImage();
	else
		SetTexture(_diffuseTexture);
}

void Component_Material::OnGUI()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Enabled", &enabled)) {}

		ImGui::SameLine();
		if (ImGui::Checkbox("Checkers Image Active", &checkersImageActive))
		{
			if (checkersImageActive)
			{
				AssignCheckersImage();
			}
			else
			{
				if (_diffuseTexture != nullptr)
					SetTexture(_diffuseTexture);
				else
					checkersImageActive = true;
			}
		}

		ImGui::Separator();

		if(_diffuseTexture != nullptr && checkersImageActive == false)
		{
			ImGui::Image((ImTextureID)_diffuseTexture->GetGpuID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					GuiAssets* assets_window = (GuiAssets*)App->gui->windows[ASSETS_WINDOW];
					const char* file = assets_window->GetFileAt(payload_n);
					Resource* possible_texture = App->resources->RequestResource(App->resources->Find(file));

					if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE)
						_diffuseTexture = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}
				
			ImGui::SameLine();

			if (ImGui::Button("Delete Texture"))
			{
				_diffuseTexture = nullptr;
				AssignCheckersImage();
			}

			ImGui::Spacing();

			ImGui::Text("Texture Path: %s", _diffuseTexture->assetsFile.c_str());

			ImGui::Text("Width: %d Height: %d", _diffuseTexture->GetWidth(), _diffuseTexture->GetHeight());
		}
		else
		{
			ImGui::Image((ImTextureID)checkersID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					GuiAssets* assets_window = (GuiAssets*)App->gui->windows[ASSETS_WINDOW];
					const char* file = assets_window->GetFileAt(payload_n);
					Resource* possible_texture = App->resources->RequestResource(App->resources->Find(file));

					if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE)
						SetTexture(dynamic_cast<ResourceTexture*>(possible_texture));
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::Text("UID: %d", resourceUID);
	}
}

void Component_Material::SetTexture(ResourceTexture* texture)
{
	_diffuseTexture = texture;
	checkersImageActive = false;
}

void Component_Material::AssignCheckersImage()
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

ResourceTexture* Component_Material::GetDiffuseTexture()
{
	return _diffuseTexture;
}
