#ifndef __UICONSOLE_H__
#define __UICONSOLE_H__

#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "UIWindow.h"
#include "ImGui/imgui.h"

class UIConsole : public UIWindow
{
public:
	UIConsole();
	~UIConsole();

	bool Start();
	void Draw();
	bool CleanUp();
};

#endif // !__UICONSOLE_H__