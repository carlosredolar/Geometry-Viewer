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
		visible = DrawModelImportingWindow();
	}
	else if (currentResourceType == ResourceType::RESOURCE_TEXTURE) 
	{
		visible = DrawTextureImportingWindow();
	}
	else
	{
		LOG_ERROR("Trying to import invalid file %s", fileToImport);
	}
}

void GuiImport::Enable(const char* file, ResourceType resourceType)
{
	fileToImport = file;
	currentResourceType = resourceType;
	visible = true;
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

		ImGui::DragFloat("Global Scale", &modelImportSettings.globalScale, 0.01f, 0.0f, 100.0f);

		const char* possibleAxis[] = { "X", "Y", "Z", "-X", "-Y", "-Z" };
		int forwardAxis = (int)modelImportSettings.forwardAxis;
		if (ImGui::Combo("Forward Axis", &forwardAxis, possibleAxis, 6))
			modelImportSettings.forwardAxis = (Axis)forwardAxis;

		int upAxis = (int)modelImportSettings.upAxis;
		if (ImGui::Combo("Up Axis", &upAxis, possibleAxis, 6))
			modelImportSettings.upAxis = (Axis)upAxis;

		ImGui::Checkbox("Ignore Cameras", &modelImportSettings.ignoreCameras);

		ImGui::Spacing();

		ImGui::Checkbox("Ignore Lights", &modelImportSettings.ignoreLights);

		if (ImGui::Button("OK", ImVec2(40, 20))) 
		{
			App->resources->modelImportSettings = modelImportSettings;
			App->resources->ImportFile(fileToImport);
			ret = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL", ImVec2(80, 20))) 
			ret = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			modelImportSettings = ModelImportingSettings();
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

		const char* textureWrapSettings[] = { "Clamp To Border", "Clamp", "Repeat", "Mirrored Repeat" };
		int texture_wrap = (int)textureImportSettings.textureWrap;
		if (ImGui::Combo("Texture Wrap", &texture_wrap, textureWrapSettings, 4))
			textureImportSettings.textureWrap = (TextureWrap)texture_wrap;

		const char* textureFilteringSettings[] = { "Nearest", "Linear" };
		int texture_filtering = (int)textureImportSettings.textureFiltering;
		if (ImGui::Combo("Texture Filtering", &texture_filtering, textureFilteringSettings, 2))
			textureImportSettings.textureFiltering = (TextureFiltering)texture_filtering;

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Columns(3);
		ImGui::Checkbox("Flip", &textureImportSettings.flip);
		ImGui::Checkbox("Alienify", &textureImportSettings.alienify);
		ImGui::Checkbox("Equalize", &textureImportSettings.equalize);

		ImGui::NextColumn();
		ImGui::Checkbox("Blur Average", &textureImportSettings.blurAverage);
		ImGui::Checkbox("Blur Gaussian", &textureImportSettings.blurGaussian);
		ImGui::Checkbox("Negativity", &textureImportSettings.negativity);

		ImGui::NextColumn();
		ImGui::Checkbox("Noise", &textureImportSettings.noise);
		ImGui::Checkbox("Pixelize", &textureImportSettings.pixelize);
		ImGui::Checkbox("Sharpening", &textureImportSettings.sharpening);

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Columns(1);
		ImGui::SliderFloat("Contrast", &textureImportSettings.contrast, 0.0f, 1.7f);
		ImGui::SliderFloat("Gamma Correction", &textureImportSettings.gammaCorrection, 0.0f, 2.0f);

		if(textureImportSettings.blurAverage)
			ImGui::SliderInt("Blur Average Iterations", &textureImportSettings.blur_average_iterations, 1, 10);

		if (textureImportSettings.blurGaussian)
			ImGui::SliderInt("Blur Gaussian Iterations", &textureImportSettings.blurGaussianIterations, 1, 10);
		
		ImGui::SliderFloat("Noise Tolerance", &textureImportSettings.noiseTolerance, 0.0f, 1.0f);
		ImGui::SliderInt("Pixel Size", &textureImportSettings.pixelizeSize, 0, 20);
		ImGui::SliderFloat("Sharpening factor", &textureImportSettings.sharpeningFactor, 0.0f, 2.5f);
		ImGui::SliderInt("Sharpening Iterations", &textureImportSettings.sharpeningIterations, 1, 10);


		ImGui::Spacing();
		if (ImGui::Button("OK", ImVec2(40, 20))) 
		{
			App->resources->textureImportSettings = textureImportSettings;
			App->resources->ImportFile(fileToImport);
			ret = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL", ImVec2(80, 20)))
			ret = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			textureImportSettings = TextureImportingSettings();
	}
	ImGui::End();

	return ret;
}
