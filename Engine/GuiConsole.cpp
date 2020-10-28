#include "Application.h"
#include "Globals.h"
#include "GuiWindow.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "Glew/include/glew.h"
#include "GuiConsole.h"
#include "ModuleGui.h"

using namespace ImGui;

GuiConsole::GuiConsole() : GuiWindow()
{
	is_on = true;
}

GuiConsole::~GuiConsole()
{}

bool GuiConsole::Start()
{
	bool ret = true;

	return ret;
}

bool GuiConsole::CleanUp()
{
	bool ret = true;

	return ret;
}

void GuiConsole::Draw()
{
	Begin("Console", &is_on);

	App->gui->DebugConsole();

	End();
}