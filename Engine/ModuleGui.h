#pragma once
#include "Module.h"
#include "Globals.h"
#include "GuiWindow.h"

//ImGui includes
#include "Imgui/imgui.h"

class GuiWindow;
class GuiConsole;
class GuiConfiguration;
class GuiInspector;
class GuiHierarchy;
class GuiImport;

class ModuleGui : public Module
{
public:
	ModuleGui(bool start_enabled = true);
	~ModuleGui();

	bool Start();
	update_status DockSpace(bool* p_open);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	update_status Draw();

	bool CleanUp();

	void SelectGameObject(GameObject* gO);
	void ConsoleLog(const char* text, int warningLevel);
	void CleanLog();
	void DebugConsole();

	std::vector<GuiWindow*> ui_windows;
	ImGuiTextBuffer debug_console_buff;
	std::vector<ImVec4> debug_console_color_buff;

private:
	bool* dockingwindow;
	bool demo;

	//UI Windows
	GuiConsole* ui_console = nullptr;
	GuiConfiguration* ui_configuration = nullptr;
	GuiInspector* ui_inspector = nullptr;
	GuiHierarchy* ui_hierarchy = nullptr;
	GuiImport* ui_import = nullptr;
};