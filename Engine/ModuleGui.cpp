#include "Application.h"
#include "ModuleGui.h"
#include "UIConsole.h"

#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_impl_sdl.h"
#include "Imgui/imgui_impl_opengl3.h"

using namespace ImGui;

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Window 3 variables
	f = 0.5;
	strncpy(buf, "Insert a text", 20);

	// Window 4 variables
	my_color[0] = 1;
	my_color[1] = 1;
	my_color[2] = 1;
	my_color[3] = 1;

	ui_windows.push_back(ui_console = new UIConsole());
}

ModuleGui::~ModuleGui()
{}

bool ModuleGui::Start() 
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	ui_console->Start();

	return true;
}

update_status ModuleGui::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	//NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGui::Update(float dt) 
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	NewFrame();

	// Window 1
	if (Begin("Test1", NULL)) {
		Text("Text window 1");
		Button("Shit");
		End();
	}

	// Window 2
	Begin("Test2", NULL);
	Text("Text window 2");
	End();

	// Window 3
	Begin("Test3", NULL);
	Text("Hello, world %d", 123);
	if (Button("Save"))
	{
		//MySaveFunction();
	}
	InputText("string", buf, IM_ARRAYSIZE(buf));
	SliderFloat("float", &f, 0.0f, 1.0f);
	End();

	// Window 4
	// If nullptr is a bool, a close icon in the window appears
	Begin("My First Tool", nullptr, ImGuiWindowFlags_MenuBar);
	if (BeginMenuBar())
	{
		if (BeginMenu("File"))
		{
			if (MenuItem("Open.."))
			{
				/* Do stuff */
			}
			if (MenuItem("Save", "Ctrl+S"))
			{
				/* Do stuff */
			}
			if (MenuItem("Close", "Ctrl+W"))
			{
				/* Do stuff */
			}
			ImGui::EndMenu();
		}
		EndMenuBar();
	}

	// Edit a color (stored as 4 floats)
	ColorEdit4("Color", my_color);

	// Plot some values
	const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
	PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

	// Display contents in a scrolling region
	TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	BeginChild("Scrolling");
	for (int n = 0; n < 50; n++)
		Text("%04d: Some text", n);
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
				ui_console->Activate();
			}
			ImGui::EndMenu();
		}
		if (BeginMenu("Help"))
		{
			PushStyleColor(ImGuiCol_Text, ImVec4(0.972, 0.105, 0.105, 1.f));

			if (MenuItem("Our Github"))
			{

				// Github link:
				ShellExecuteA(NULL, "open", "https://github.com/carlosredolar/R-Engine", NULL, NULL, SW_SHOWNORMAL);
			}


			PopStyleColor();
			ImGui::EndMenu();
		}

		EndMainMenuBar();
	}

	for (int i = 0; i < ui_windows.capacity(); i++)
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
	for (int i = 0; i < ui_windows.capacity(); i++)
	{
		ui_windows[i]->~UIWindow();
	}
	ui_windows.clear();

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