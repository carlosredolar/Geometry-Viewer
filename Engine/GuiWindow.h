#pragma once

enum WindowType {
	HIERARCHY_WINDOW,
	INSPECTOR_WINDOW,
	SCENE_WINDOW,
	//CONSOLE_WINDOW,
	ASSETS_WINDOW,
	CONFIGURATION_WINDOW,
	IMPORT_WINDOW,
	MAX_WINDOWS
};

class GuiWindow {
public:
	GuiWindow() : visible(false){};
	virtual ~GuiWindow() {};
	virtual bool Init() { return true; };
	virtual void Draw() = 0;

public:
	bool visible;
	WindowType type = MAX_WINDOWS;
};
