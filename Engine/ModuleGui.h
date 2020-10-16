#pragma once
#include "Module.h"
#include "Globals.h"
#include "UIWindow.h"

//ImGui includes
#include "Imgui/imgui.h"

class UIWindow;
class UIConsole;

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

	std::vector<UIWindow*> ui_windows;
	ImGuiTextBuffer debug_console_buff;

private:
	// Window 3 variables
	float f;
	char buf[20];

	// Window 4 variables
	float my_color[4];

	//UI Windows
	UIConsole* ui_console = nullptr;
};