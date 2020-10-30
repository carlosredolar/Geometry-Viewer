#include "Application.h"
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