#pragma once
#include "Module.h"
#include "Globals.h"

//ImGui includes
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_impl_sdl.h"
#include "Imgui/imgui_impl_opengl3.h"

class ModuleGui : public Module
{
public:
	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private:
	// Window 3 variables
	float f;
	char buf[20];

	// Window 4 variables
	float my_color[4];
};