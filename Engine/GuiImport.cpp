#include "GuiImport.h"
#include "Application.h"
#include "ImGui/imgui.h"

GuiImport::GuiImport() : GuiWindow(), fileToImport(nullptr), currentResourceType(ResourceType::RESOURCE_UNKNOWN)
{}

GuiImport::~GuiImport()
{
	fileToImport = nullptr;
}

void GuiImport::Draw()
{
	if (currentResourceType == ResourceType::RESOURCE_MODEL) 
	{
		is_on = DrawModelImportingWindow();
	}
	else if (currentResourceType == ResourceType::RESOURCE_TEXTURE) 
	{
		is_on = DrawTextureImportingWindow();
	}
	else
	{
		ERROR_LOG("Trying to import invalid file %s", fileToImport);
	}
}

void GuiImport::Enable(const char* file, ResourceType resourceType)
{
	fileToImport = file;
	currentResourceType = resourceType;
	is_on = true;
}

bool GuiImport::DrawModelImportingWindow()
{
	bool ret = true;

	if (ImGui::Begin("Import File", NULL))
	{
		ImGui::Text("Import Model: %s", fileToImport);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::DragFloat("Global Scale", &modelImportingSettings.globalScale, 0.01f, 0.0f, 100.0f);

		const char* possible_axis[] = { "X", "Y", "Z", "-X", "-Y", "-Z" };
		int forward_axis = (int)modelImportingSettings.forwardAxis;
		if (ImGui::Combo("Forward Axis", &forward_axis, possible_axis, 6))
			modelImportingSettings.forwardAxis = (Axis)forward_axis;

		int up_axis = (int)modelImportingSettings.upAxis;
		if (ImGui::Combo("Up Axis", &up_axis, possible_axis, 6))
			modelImportingSettings.upAxis = (Axis)up_axis;

		ImGui::Checkbox("Ignore Cameras", &modelImportingSettings.ignoreCameras);

		ImGui::Spacing();

		ImGui::Checkbox("Ignore Lights", &modelImportingSettings.ignoreLights);

		if (ImGui::Button("OK", ImVec2(40, 20))) 
		{
			App->resources->modelImportingSettings = modelImportingSettings;
			App->resources->ImportFile(fileToImport);
			ret = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL", ImVec2(80, 20))) 
			ret = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			modelImportingSettings = ModelImportingSettings();
	}
	ImGui::End();

	return ret;
}

bool GuiImport::DrawTextureImportingWindow()
{
	bool ret = true;

	if (ImGui::Begin("Import File", NULL))
	{
		ImGui::Text("Import Texture: %s", fileToImport);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		const char* texture_wrap_options[] = { "Clamp To Border", "Clamp", "Repeat", "Mirrored Repeat" };
		int texture_wrap = (int)textureImportingSettings.textureWrap;
		if (ImGui::Combo("Texture Wrap", &texture_wrap, texture_wrap_options, 4))
			textureImportingSettings.textureWrap = (TextureWrap)texture_wrap;

		const char* texture_filtering_options[] = { "Nearest", "Linear" };
		int texture_filtering = (int)textureImportingSettings.textureFiltering;
		if (ImGui::Combo("Texture Filtering", &texture_filtering, texture_filtering_options, 2))
			textureImportingSettings.textureFiltering = (TextureFiltering)texture_filtering;

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Columns(3);
		ImGui::Checkbox("Flip", &textureImportingSettings.flip);
		//ImGui::SameLine();
		ImGui::Checkbox("Alienify", &textureImportingSettings.alienify);
		//ImGui::SameLine();
		ImGui::Checkbox("Equalize", &textureImportingSettings.equalize);

		ImGui::NextColumn();
		ImGui::Checkbox("Blur Average", &textureImportingSettings.blur_average);
		ImGui::Checkbox("Blur Gaussian", &textureImportingSettings.blur_gaussian);
		//ImGui::SameLine();
		ImGui::Checkbox("Negativity", &textureImportingSettings.negativity);

		ImGui::NextColumn();
		ImGui::Checkbox("Noise", &textureImportingSettings.noise);
		//ImGui::SameLine();
		ImGui::Checkbox("Pixelize", &textureImportingSettings.pixelize);
		//ImGui::SameLine();
		ImGui::Checkbox("Sharpening", &textureImportingSettings.sharpening);

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Columns(1);
		ImGui::SliderFloat("Contrast", &textureImportingSettings.contrast, 0.0f, 1.7f);
		ImGui::SliderFloat("Gamma Correction", &textureImportingSettings.gamma_correction, 0.0f, 2.0f);
		ImGui::SliderFloat("Noise Tolerance", &textureImportingSettings.noise_tolerance, 0.0f, 1.0f);
		ImGui::SliderInt("Pixel Size", &textureImportingSettings.pixelize_size, 0, 20);
		ImGui::SliderFloat("Sharpening factor", &textureImportingSettings.sharpening_factor, 0.0f, 2.5f);
		ImGui::SliderInt("Sharpening Iterations", &textureImportingSettings.sharpening_iterations, 1, 10);


		ImGui::Spacing();
		if (ImGui::Button("OK", ImVec2(40, 20))) 
		{
			App->resources->textureImportingSettings = textureImportingSettings;
			App->resources->ImportFile(fileToImport);
			ret = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL", ImVec2(80, 20)))
			ret = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			textureImportingSettings = TextureImportingSettings();
	}
	ImGui::End();

	return ret;
}
