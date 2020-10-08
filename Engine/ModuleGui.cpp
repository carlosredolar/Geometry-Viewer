#include "Application.h"
#include "ModuleGui.h"

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
}

ModuleGui::~ModuleGui()
{}

bool ModuleGui::Start() 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	return true;
}

update_status ModuleGui::Update(float dt) 
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	// Window 1
	if (ImGui::Begin("Test1", NULL)) {
		ImGui::Text("Text window 1");
		ImGui::Button("Shit");
		ImGui::End();
	}

	// Window 2
	ImGui::Begin("Test2", NULL);
	ImGui::Text("Text window 2");
	ImGui::End();

	// Window 3
	ImGui::Begin("Test3", NULL);
	ImGui::Text("Hello, world %d", 123);
	if (ImGui::Button("Save"))
	{
		//MySaveFunction();
	}
	ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	ImGui::End();

	// Window 4
	// If nullptr is a bool, a close icon in the window appears
	ImGui::Begin("My First Tool", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open.."))
			{
				/* Do stuff */
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				/* Do stuff */
			}
			if (ImGui::MenuItem("Close", "Ctrl+W"))
			{
				/* Do stuff */
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Edit a color (stored as 4 floats)
	ImGui::ColorEdit4("Color", my_color);

	// Plot some values
	const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
	ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

	// Display contents in a scrolling region
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	ImGui::BeginChild("Scrolling");
	for (int n = 0; n < 50; n++)
		ImGui::Text("%04d: Some text", n);
	ImGui::EndChild();
	ImGui::End();

	// Tool bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("New"))
			{

				// New file
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.972, 0.105, 0.105, 1.f));

			if (ImGui::MenuItem("Quit"))
			{

				// Exits the app
				return UPDATE_STOP;
			}

			ImGui::PopStyleColor();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.972, 0.105, 0.105, 1.f));

			if (ImGui::MenuItem("Our Github"))
			{

				// Github link:
				ShellExecuteA(NULL, "open", "https://github.com/carlosredolar/R-Engine", NULL, NULL, SW_SHOWNORMAL);
			}


			ImGui::PopStyleColor();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}


	//Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp() 
{
	return true;
}