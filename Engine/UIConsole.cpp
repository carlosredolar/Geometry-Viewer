#include "Application.h"
#include "Globals.h"
#include "UIWindow.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "Glew/include/glew.h"
#include "UIConsole.h"
#include "ModuleGui.h"

using namespace ImGui;

UIConsole::UIConsole() : UIWindow()
{
	is_on = false;
}

UIConsole::~UIConsole()
{}

bool UIConsole::Start()
{
	bool ret = true;

	return ret;
}

bool UIConsole::CleanUp()
{
	bool ret = true;

	return ret;
}

void UIConsole::Draw()
{
	Begin("Console", &is_on);

	App->gui->DebugConsole();

	End();
}