#pragma once
#include "Module.h"
#include "Globals.h"
#include "GuiWindow.h"

//ImGui includes
#include "Imgui/imgui.h"

class GuiWindow;
class GuiConsole;
class GuiConfiguration;

class ModuleGui : public Module
{
public:
	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void ConsoleLog(const char* text);
	void DebugConsole();

	std::vector<GuiWindow*> ui_windows;
	ImGuiTextBuffer debug_console_buff;

private:
	// Window 3 variables
	float f;
	char buf[20];

	// Window 4 variables
	float my_color[4];

	//UI Windows
	GuiConsole* ui_console = nullptr;
	GuiConfiguration* ui_configuration = nullptr;
};