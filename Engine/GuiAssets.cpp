#include "GuiAssets.h"
#include "Application.h"
#include "FileManager.h"
#include "ResourceTexture.h"
#include <algorithm>
#include "ImGui/imgui.h"

GuiAssets::GuiAssets() : GuiWindow()
{
	type = WindowType::ASSETS_WINDOW;
	currentFolder = "Assets";
}

GuiAssets::~GuiAssets() 
{
	currentFolder.clear();
}

bool GuiAssets::Init()
{
	bool ret = true;

	App->resources->LoadEngineAssets(icons);

	return ret;
}

void GuiAssets::Draw()
{
	if (ImGui::Begin("Assets", &visible)) {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysVerticalScrollbar;

        ImGui::BeginChild("Tree", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.15f, ImGui::GetContentRegionAvail().y), false, windowFlags);
		if (ImGui::Button("Assets")) currentFolder = "Assets";

		RenderDirectory("Assets", nullptr);
        ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Folder", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
		RenderFolder();
		ImGui::EndChild();
	}
	ImGui::End();
}

void GuiAssets::RenderDirectory(const char* directory, const char* filterExtension)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
	FileManager::DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{

		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str(), treeFlags))
		{

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::Button("Delete"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();

			}

			if (ImGui::IsItemClicked()) {
				currentFolder = directory;

				currentFolder.append("/");
				currentFolder.append(*it).c_str();
			}
			RenderDirectory((dir + (*it)).c_str(), filterExtension);

			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		bool ok = true;

		if (filterExtension && str.substr(str.find_last_of(".") + 1) != filterExtension) ok = false;

		if (it->find(".meta") != -1) ok = false;

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::BeginPopupContextItem()) {

				if (ImGui::Button("Delete"))
				{
					std::string file_to_delete = directory;

					file_to_delete.append("/" + str);
					App->resources->AssetToDelete(file_to_delete.c_str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();

		}
	}
}

void GuiAssets::RenderFolder()
{
	std::vector<std::string> temporalFiles;
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	FileManager::DiscoverFiles(currentFolder.c_str(), temporalFiles, dirs);

	for (size_t i = 0; i < temporalFiles.size(); ++i)
	{
		if (temporalFiles[i].find(".meta") == std::string::npos) 
		{
			files.push_back(temporalFiles[i]);
		}
	}

	temporalFiles.clear();

	int iconsAmount = files.size() + dirs.size();
	int drawnIcons = 0;

	int columns = floor(ImGui::GetContentRegionAvailWidth() / 100.0f);
	int rows = ceil(files.size() + dirs.size() / (float)columns);

	int drawnColumns = 0;
	int drawnRows = 0;
	int drawnDirs = 0;
	int drawnFiles = 0;

	bool ret = true;

	for (size_t r = 0; r < rows && drawnIcons < iconsAmount && ret; r++)
	{
		ImGui::Columns(columns, 0, false);
		for (size_t c = 0; c < columns && drawnIcons < iconsAmount && ret; c++)
		{
			if (drawnDirs < dirs.size()) 
			{
				std::string path = currentFolder + "/" + dirs[drawnDirs];
				ret = RenderIcon(path.c_str(), drawnIcons, true);
				drawnDirs++;
			}
			else
			{
				std::string path = currentFolder + "/" + files[drawnFiles];
				RenderIcon(path.c_str(), drawnIcons, false);
				drawnFiles++;
			}
			ImGui::NextColumn();
			drawnIcons++;
		}
		ImGui::Columns(1);
	}
}

bool GuiAssets::RenderIcon(const char* path, int id, bool isFolder)
{
	bool ret = true;

	if (isFolder)
	{
		ImGui::PushID(id);
		if (ImGui::ImageButton((ImTextureID)icons.folder->GetGpuID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0), 0 ,ImVec4(0,0,0,1))) 
		{
			currentFolder = path;
			ret = false;
		}
		ImGui::Text("%s", FileManager::GetFileName(path).c_str());

		ImGui::PopID();
	}
	else
	{
		ImGui::PushID(id);
		ImGui::ImageButton((ImTextureID)icons.model->GetGpuID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0, 0, 0, 1));

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ASSETS", &id, sizeof(int));
			ImGui::Text("%s", FileManager::GetFileNameAndExtension(path).c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Button("Delete"))
			{
				App->resources->AssetToDelete(path);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		std::string fileName = FileManager::GetFileNameAndExtension(path);

		if (fileName.find(".fbx") != std::string::npos)
		{
			ImGui::SameLine();
			if (ImGui::Button("->")) ImGui::OpenPopup("Meshes");

			if (ImGui::BeginPopup("Meshes", ImGuiWindowFlags_NoMove))
			{
				std::string model = currentFolder + "/" + fileName;
				const char* library_path = App->resources->Find(App->resources->GetUIDFromMeta(model.append(".meta").c_str()));

				std::vector<uint> meshes;
				std::vector<uint> materials;

				ModelImporter::ExtractInternalResources(library_path, meshes, materials);

				for (size_t m = 0; m < meshes.size(); m++)
				{
					ImGui::PushID(meshes[m]);
					ResourceData meshData = App->resources->RequestResourceData(meshes[m]);
					ImGui::Text("%s", meshData.name.c_str());

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("MESHES", &(meshes[m]), sizeof(int));
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();

				}
				ImGui::EndPopup();

			}
		}
		ImGui::Text("%s", fileName.c_str());
		ImGui::PopID();

	}
	return ret;
}

const char* GuiAssets::GetFileAt(int i)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	FileManager::DiscoverFiles(currentFolder.c_str(), files, dirs);
	std::string file = currentFolder + "/"+ files[i - dirs.size()];
	char* file_cstr = new char[256];
	sprintf_s(file_cstr, 256,"%s",file.c_str());

	return file_cstr;
}
