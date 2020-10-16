#ifndef __GUICONSOLE_H__
#define __GUICONSOLE_H__

#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "GuiWindow.h"
#include "ImGui/imgui.h"

class GuiConsole : public GuiWindow
{
public:
	GuiConsole();
	~GuiConsole();

	bool Start();
	void Draw();
	bool CleanUp();
};

#endif // !__GUICONSOLE_H__