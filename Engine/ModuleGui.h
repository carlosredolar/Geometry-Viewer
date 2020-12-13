#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "GuiWindow.h"

#include <vector>
#include <string>

typedef int GLint;

struct messageToLog {
	std::string text;
	int warning_level;
};

class ModuleGui : public Module 
{
public:
	ModuleGui(bool start_enabled = true);
	~ModuleGui();

	bool Init();
	bool Start() override;
	bool LoadConfig(JsonObj& config) override;
	update_status Update(float dt);
	update_status Draw();
	bool CleanUp();

	bool IsSceneFocused();
	bool MouseOnScene();
	void AddConsoleLog(const char* log, int warning_level);

	void ScreenResized(ImVec2 window_size);
	void LoadFile(const char* filter_extension, const char* from_dir);
	void SaveFile(const char* filter_extension, const char* from_dir);
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);

private:
	//Dock Space
	update_status DockSpace(bool* p_open);
	bool CreateMenuBar();

	void PlayStopButtons();

public:
	ImVec2 sceneRenderSize;
	ImVec2 sceneWindowOrigin;
	ImVec2 mouseScenePosition;
	bool sceneWindowFocused;
	GuiWindow* windows[MAX_WINDOWS];

private:

	bool show_project_window;
	bool consoleWindow;
	bool resourcesWindow;
	bool newLog;

	//edit subwindows
	bool showGameButtons;

	//menus
	bool* open_dockspace;

	std::vector<messageToLog> consoleLog;

	enum
	{
		closed,
		opened,
		ready_to_close
	} file_dialog = closed;

	enum class SceneOperation
	{
		SAVE,
		LOAD, 
		NONE
	}scene_operation = SceneOperation::NONE;

	bool in_modal = false;
	char selected_folder[256];
	char scene_name[128];
	char selected_file[256];
};

#endif // !_EDITOR_H_

