#include "Application.h"
#include "ModuleGui.h"
#include "GuiConsole.h"
#include "GuiConfiguration.h"
#include "GuiInspector.h"
#include "ModuleImport.h"
#include "ModuleScene.h"

#include "ImGui/imconfig.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

using namespace ImGui;

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ui_windows.push_back(ui_console = new GuiConsole());
	ui_windows.push_back(ui_configuration = new GuiConfiguration());
	ui_windows.push_back(ui_inspector = new GuiInspector());
}

ModuleGui::~ModuleGui()
{}

bool ModuleGui::Start() 
{
	CreateContext();
	ImGuiIO& io = GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	ui_console->Start();
	ui_configuration->Start();
	ui_inspector->Start();

	return true;
}

update_status ModuleGui::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGui::Update(float dt) 
{
	DockSpace(dockingwindow);

	if (demo) {
		ShowDemoWindow(&demo);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		if (ui_inspector->is_on) ui_inspector->Select(App->scene->GetGameObject("Baker_house"));
	}

	// Window 2
	// If nullptr is a bool, a close icon in the window appears
	Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);
	
	TextColored(ImVec4(1, 1, 0, 1), "Scene");
	BeginChild("Scrolling");
	for (int n = 0; n < 50; n++)
		Text("%02d: Object", n);
	EndChild();
	End();

	// Tool bar
	if (BeginMainMenuBar())
	{
		if (BeginMenu("File"))
		{

			if (MenuItem("New"))
			{

				// New file
			}

			PushStyleColor(ImGuiCol_Text, ImVec4(0.972, 0.105, 0.105, 1.f));

			if (MenuItem("Quit"))
			{

				// Exits the app
				return UPDATE_STOP;
			}

			PopStyleColor();
			ImGui::EndMenu();
		}
		if (BeginMenu("View"))
		{
			if (MenuItem("Console")) 
			{
				ui_console->is_on = !ui_console->is_on;
			}
			if (MenuItem("Configuration"))
			{
				ui_configuration->is_on = !ui_configuration->is_on;
			}
			ImGui::EndMenu();
		}
		if (BeginMenu("Add"))
		{
			if (BeginMenu("Primitives"))
			{
				if (MenuItem("Cube"))
				{
					App->importer->LoadMesh("Assets/Primitives/Cube.fbx");
				}
				if (MenuItem("Sphere"))
				{
					App->importer->LoadMesh("Assets/Primitives/Sphere.fbx");
				}
				if (MenuItem("Cylinder"))
				{
					App->importer->LoadMesh("Assets/Primitives/Cylinder.fbx");
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (BeginMenu("About"))
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

			GLint gl_major_version, gl_minor_version;
			glGetIntegerv(GL_MAJOR_VERSION, &gl_major_version);
			glGetIntegerv(GL_MINOR_VERSION, &gl_minor_version);
			ImGui::Text("3rd party libraries used: ");
			ImGui::BulletText("SDL"); 
			ImGui::SameLine();
			ImGui::Text("%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
			ImGui::BulletText("SDL Mixer ");
			ImGui::SameLine();
			ImGui::Text("2.0.0");
			ImGui::BulletText("ImGui "); 
			ImGui::SameLine();
			ImGui::Text("%s", ImGui::GetVersion());
			ImGui::BulletText("OpenGL "); 
			ImGui::SameLine();
			ImGui::Text("%d.%d", gl_major_version, gl_minor_version);
			ImGui::BulletText("Glew ");	
			ImGui::SameLine();
			ImGui::Text("%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
			ImGui::BulletText("MathGeoLib"); 
			ImGui::SameLine();
			ImGui::Text("1.5");
			ImGui::BulletText("Assimp "); 
			ImGui::SameLine();
			ImGui::Text("3.1.1");	
			ImGui::BulletText("Devil ");
			ImGui::SameLine();
			ImGui::Text("1.7.8");

			ImGui::Separator();

			ImGui::Text("License: ");
			ImGui::Spacing();
			ImGui::TextWrapped("MIT License");
			ImGui::Spacing();
			ImGui::TextWrapped("Copyright(c) 2020 pauraurell & pgalmor");
			ImGui::Spacing();
			ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the Software), to deal "
				"in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell "
				"copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:");
			ImGui::Spacing();
			ImGui::TextWrapped("The above copyright noticeand this permission notice shall be included in all"
				"copies or substantial portions of the Software.");
			ImGui::Spacing();
			ImGui::TextWrapped("THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
				"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
				"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

		
			ImGui::EndMenu();
		}

		EndMainMenuBar();
	}

	for (int i = 0; i < ui_windows.size(); i++)
	{
		if (ui_windows[i]->IsActive())
			ui_windows[i]->Draw();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleGui::PostUpdate(float dt)
{
	//Rendering
	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());

	SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

	UpdatePlatformWindows();
	RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp() 
{
	for (int i = 0; i < ui_windows.capacity(); i++) //size instead of capacity
	{
		//ui_windows[i]->is_on = false;
		ui_windows[i]->~GuiWindow();
	}
	ui_windows.clear();

	debug_console_buff.clear();

	return true;
}

void ModuleGui::ConsoleLog(const char* text)
{
	debug_console_buff.appendf(text);
}

void ModuleGui::DebugConsole()
{
	BeginChild("Console Log");
	TextUnformatted(debug_console_buff.begin());
	EndChild();
}

update_status ModuleGui::DockSpace(bool* p_open)
{
	update_status ret = UPDATE_CONTINUE;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
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

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();

	return ret;
}