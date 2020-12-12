#include "Application.h"
#include "ModuleGui.h"
#include "ModuleJson.h"
#include "Component_Mesh.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "FileManager.h"
#include "Component_Camera.h"
#include "Time.h"

#include <vector>
#include <string>
#include <algorithm>

#include "glew/include/glew.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "Assimp/include/version.h"

//Windows
#include "GuiHierarchy.h"
#include "GuiInspector.h"
#include "GuiScene.h"
#include "GuiAssets.h"
#include "GuiConfiguration.h"
#include "GuiImport.h"

#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

ModuleGui::ModuleGui(bool start_enabled) : Module(start_enabled)
{
	name = "editor";

	sceneWindowFocused = false;
	showGameButtons = true;

	image_size = { 0,0 };

	windows[HIERARCHY_WINDOW] = new GuiHierarchy();
	windows[INSPECTOR_WINDOW] = new GuiInspector();
	windows[SCENE_WINDOW] = new GuiScene();
	windows[ASSETS_WINDOW] = new GuiAssets();
	windows[CONFIGURATION_WINDOW] = new GuiConfiguration();
	windows[IMPORT_WINDOW] = new GuiImport();

	//CONSOLE_WINDOW,
	scene_name[0] = '\0';
	selected_file[0] = '\0';
	selected_folder[0] = '\0';
}

ModuleGui::~ModuleGui() {}

bool ModuleGui::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	return true;
}

bool ModuleGui::Start()
{
	bool ret = true;

	for (size_t i = 0; i < MAX_WINDOWS; i++)
		windows[i]->Init();

	return ret;
}

update_status ModuleGui::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ret = DockSpace(open_dockspace);

	return ret;
}

update_status ModuleGui::Draw()
{
	for (size_t i = 0; i < MAX_WINDOWS; i++)
	{
		if(windows[i]->visible) windows[i]->Draw();
	}

	PlayStopButtons();

	//Console
	if (show_console_window)
	{
		if (ImGui::Begin("Console", &show_console_window, ImGuiWindowFlags_MenuBar)) 
		{
			if (ImGui::BeginMenuBar()) 
			{
				if (ImGui::MenuItem("Clear"))
				{
					console_log.clear();
				}
				ImGui::EndMenuBar();
			}

			for (int i = 0; i < console_log.size(); i++)
			{
				if (console_log[i].warning_level == 0) //Normal log
				{
					ImGui::Text(console_log[i].log_text.c_str());
				}
				else if (console_log[i].warning_level == 1) //Warning
				{
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), console_log[i].log_text.c_str());
				}
				else //Error
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), console_log[i].log_text.c_str());
			}

			if (newLog)
			{
				ImGui::SetScrollHereY(1.0f);
				newLog = false;
			}
		}
		ImGui::End();
	}

	if (resources_window)
	{
		if (ImGui::Begin("Resources", &resources_window))
		{
			App->resources->OnGUI();
		}
		ImGui::End();
	}

	if (file_dialog == opened)
	{
		if (scene_operation == SceneOperation::LOAD)
			LoadFile(".scener", "Library/");
		else if (scene_operation == SceneOperation::SAVE)
			SaveFile(".scener", "Library/");
	}

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	console_log.clear();

	for (size_t i = 0; i < MAX_WINDOWS; i++)
	{
		delete windows[i];
		windows[i] = nullptr;
	}

	return true;
}

bool ModuleGui::LoadConfig(JsonObj& config)
{
	JsonArray jsonWindows(config.GetArray("windows"));
	
	JsonObj window = jsonWindows.GetObjectInArray("scene");
	windows[SCENE_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("inspector");
	windows[INSPECTOR_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("hierarchy");
	windows[HIERARCHY_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("assets");
	windows[ASSETS_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("console");
	show_console_window = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("resources");
	resources_window = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("configuration");
	windows[CONFIGURATION_WINDOW]->visible = window.GetBool("visible");
	
	return true;
}

bool ModuleGui::IsSceneFocused()
{
	return sceneWindowFocused;
}

bool ModuleGui::MouseOnScene()
{
	return mouseScenePosition.x > 0 && mouseScenePosition.x < image_size.x 
		   && mouseScenePosition.y > 0 && mouseScenePosition.y < image_size.y;
}

void ModuleGui::AddConsoleLog(const char* log, int warning_level)
{
	log_message message = { log, warning_level };
	console_log.push_back(message);
	
	newLog = true;
}

update_status ModuleGui::DockSpace(bool* p_open) 
{
	update_status ret = UPDATE_CONTINUE;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	if(ImGui::Begin("DockSpace", p_open, window_flags)){
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		//main bar
		if (CreateMenuBar())
			ret = UPDATE_CONTINUE;
		else
			ret = UPDATE_STOP;
	}
	ImGui::End();

	return ret;
}

bool ModuleGui::CreateMenuBar() {
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene"))
			{
				file_dialog = opened;
				scene_operation = SceneOperation::SAVE;
			}
			else if (ImGui::MenuItem("Load Scene"))
			{
				file_dialog = opened;
				scene_operation = SceneOperation::LOAD;
			}
			else if (ImGui::MenuItem("Exit"))
			{
				ret = false;
			}
			ImGui::EndMenu();
		}
	
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Empty Object"))
			{
				App->scene->AddGameObject(new GameObject());
			}
			if (ImGui::BeginMenu("Primitives"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					App->scene->AddGameObject(App->resources->RequestGameObject("Assets/Models/Primitives/Cube.fbx"));
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					App->scene->AddGameObject(App->resources->RequestGameObject("Assets/Models/Primitives/cylinder.fbx"));
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					App->scene->AddGameObject(App->resources->RequestGameObject("Assets/Models/Primitives/sphere.fbx"));
				}
				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Inspector", NULL, windows[INSPECTOR_WINDOW]->visible))
				windows[INSPECTOR_WINDOW]->visible = !windows[INSPECTOR_WINDOW]->visible;
			else if (ImGui::MenuItem("Hierarchy", NULL, windows[HIERARCHY_WINDOW]->visible))
				windows[HIERARCHY_WINDOW]->visible = !windows[HIERARCHY_WINDOW]->visible;
			else if (ImGui::MenuItem("Scene", NULL, windows[SCENE_WINDOW]->visible))
				windows[SCENE_WINDOW]->visible = !windows[SCENE_WINDOW]->visible;
			else if (ImGui::MenuItem("Assets", NULL, windows[ASSETS_WINDOW]->visible))
				windows[ASSETS_WINDOW]->visible = !windows[ASSETS_WINDOW]->visible;
			else if (ImGui::MenuItem("Console", NULL, show_console_window))
				show_console_window = !show_console_window;
			else if (ImGui::MenuItem("Resources", NULL, resources_window))
				resources_window = !resources_window;
			else if (ImGui::MenuItem("Configuration", NULL, windows[CONFIGURATION_WINDOW]->visible))
				windows[CONFIGURATION_WINDOW]->visible = !windows[CONFIGURATION_WINDOW]->visible;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			ImGui::TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "R-Engine");
			ImGui::Separator();

			if (ImGui::SmallButton("Github page")) {
				ShellExecuteA(NULL, "open", "https://github.com/carlosredolar/R-Engine", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::TextWrapped("3D Game Engine");
			ImGui::TextWrapped("By");
			ImGui::SameLine();
			if (ImGui::SmallButton("Oscar Royo")) {
				ShellExecuteA(NULL, "open", "https://github.com/oscarroyo4", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::SameLine();
			ImGui::TextWrapped("and");
			ImGui::SameLine();
			if (ImGui::SmallButton("Carlos Redolar")) {
				ShellExecuteA(NULL, "open", "https://github.com/carlosredolar", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Separator();

			//SDL Version
			ImGui::BulletText("SDL %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

			//OpenGL Version
			static GLint openGL_major = -1;
			static GLint openGL_minor = -1;

			if (openGL_major == -1)
				glGetIntegerv(GL_MAJOR_VERSION, &openGL_major);
			if (openGL_minor == -1)
				glGetIntegerv(GL_MINOR_VERSION, &openGL_minor);

			ImGui::BulletText("OpenGL %d.%d", openGL_major, openGL_minor);

			//MathGeoLib
			ImGui::BulletText("MathGeoLib 1.5");

			//ImGui
			static const char* imgui_version = { ImGui::GetVersion() };
			ImGui::BulletText("ImGui %s", imgui_version);

			//Glew
			ImGui::BulletText("Glew %d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);

			ImGui::BulletText("DevIL 1.8.0");

			ImGui::BulletText("Assimp %d.%d.%d", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

			static std::string physfs_version;
			FileManager::GetPhysFSVersion(physfs_version);
			ImGui::BulletText("PhysFS %s", physfs_version.c_str());

			ImGui::BulletText("Parson 1.1.0");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("License: ");
			ImGui::Spacing();

			ImGui::Text("MIT License");
			ImGui::Spacing();

			ImGui::TextWrapped("Copyright (c) 2020 oscarroyo4 & carlosredolar");
			ImGui::Spacing();
			ImGui::TextWrapped(
				"Permission is hereby granted, free of charge, to any person obtaining a copy"
				"of this software and associated documentation files Genesis Engine, to deal"
				"in the Software without restriction, including without limitation the rights"
				"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell"
				"copies of the Software, and to permit persons to whom the Software is"
				"furnished to do so, subject to the following conditions : ");
			ImGui::Spacing();

			ImGui::TextWrapped(
				"The above copyright notice and this permission notice shall be included in all"
				"copies or substantial portions of the Software.");
			ImGui::Spacing();

			ImGui::TextWrapped(
				"THE SOFTWARE IS PROVIDED 'AS I', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
				"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
				"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE"
				"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
				"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
				"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"
				"SOFTWARE.");
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return ret;
}

void ModuleGui::PlayStopButtons()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowSize(ImVec2(130, 40));
	if (ImGui::Begin("Game Buttons", &showGameButtons, flags))
	{
		ImGui::Columns(4);
		ImGui::NextColumn();
		if (App->in_game == false)
		{
			if (ImGui::Button("Play", ImVec2(40, 20)))
				App->StartGame();
		}
		else {
			if (ImGui::Button("Stop", ImVec2(40, 20)))
				App->StopGame();
		}

		ImGui::NextColumn();
		if (Time::gameClock.paused) 
		{
			if (ImGui::Button("Resume", ImVec2(45, 20)))
				Time::gameClock.Resume();
		}
		else 
		{
			if (ImGui::Button("Pause", ImVec2(45, 20))) 
				Time::gameClock.Pause();
		}

	}
	ImGui::End();
}

void ModuleGui::LoadFile(const char* filter_extension, const char* from_dir)
{
	ImGui::OpenPopup("Load File");
	if (ImGui::BeginPopupModal("Load File", nullptr))
	{
		in_modal = true;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);
		 DrawDirectoryRecursive(from_dir, filter_extension);
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(250.f);
		if (ImGui::InputText("##file_selector", selected_file, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			file_dialog = ready_to_close;

		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Ok", ImVec2(50, 20))) {
			file_dialog = ready_to_close;
			App->Load(selected_file);
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
			selected_file[0] = '\0';
		}

		ImGui::EndPopup();
	}
}

void ModuleGui::SaveFile(const char* filter_extension, const char* from_dir)
{
	ImGui::OpenPopup("Save File");
	if (ImGui::BeginPopupModal("Save File", nullptr))
	{
		in_modal = true;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);
		DrawDirectoryRecursive(from_dir, filter_extension);
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(250.f);
		if (ImGui::InputText("##file_selector", scene_name, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			file_dialog = ready_to_close;
			sprintf_s(selected_file, 128, "%s/%s.scener", selected_folder, scene_name);
			App->Save(selected_file);
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Ok", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
			sprintf_s(selected_file, 128, "%s/%s.scener", selected_folder, scene_name);
			App->Save(selected_file);
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
			selected_file[0] = '\0';
		}

		ImGui::EndPopup();
	}
}

void ModuleGui::DrawDirectoryRecursive(const char* directory, const char* filter_extension)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

	std::string dir((directory) ? directory : "");

	FileManager::DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		std::string folder = (dir + (*it)).c_str();
		if (strcmp(folder.c_str(), selected_folder) == 0)
			flags = ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(folder.c_str(), flags, "%s/", (*it).c_str()))
		{
			flags = ImGuiTreeNodeFlags_None;

			if(ImGui::IsItemClicked())
				sprintf_s(selected_folder, 256, "%s%s", directory, (*it).c_str());

			DrawDirectoryRecursive((dir + (*it)).c_str(), filter_extension);
			ImGui::TreePop();
		}

		flags = ImGuiTreeNodeFlags_None;
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool ok = true;

		if (filter_extension && str.find(filter_extension) == std::string::npos)
			ok = false;

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::IsItemClicked()) {
				sprintf_s(selected_file, 256, "%s/%s", dir.c_str(), str.c_str());

				if (ImGui::IsMouseDoubleClicked(0))
				{
					file_dialog = ready_to_close;
					App->Load(selected_file);
				}
			}

			ImGui::TreePop();
		}
	}
}

void ModuleGui::OnResize(ImVec2 window_size)
{
	image_size = window_size;

	App->camera->OnResize(image_size.x, image_size.y);
	App->renderer3D->OnResize(image_size.x, image_size.y);
}

