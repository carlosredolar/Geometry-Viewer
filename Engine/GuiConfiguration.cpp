#include "GuiConfiguration.h"
#include "Application.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Glew/include/glew.h"
#include "Component_Camera.h"
#include "Time.h"


GuiConfiguration::GuiConfiguration() : GuiWindow() 
{
	type = WindowType::CONFIGURATION_WINDOW;

	fps_log.resize(100, 0);
	ms_log.resize(100, 0);

	current_theme = 1;
}

GuiConfiguration::~GuiConfiguration() 
{
	fps_log.clear();
	ms_log.clear();
}

void GuiConfiguration::Draw()
{
	if (ImGui::Begin("Configuration", &visible))
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			static int fps_cap = App->GetFPSCap();
			if (ImGui::SliderInt("Max FPS", &fps_cap, 10, 120)) {
				App->SetFPSCap(fps_cap);
			}

			char title[25];
			//FPS graph
			fps_log.erase(fps_log.begin());
			fps_log.push_back(App->GetFPS());
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

			//Ms graph
			ms_log.erase(ms_log.begin());
			ms_log.push_back(App->GetLastDt() * 1000);
			sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			static float brightness = App->window->GetBrightness();
			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
				App->window->SetBrightness(brightness);

			static int width, height;
			App->window->GetSize(width, height);

			if ((ImGui::SliderInt("Width", &width, 640, 3840) || ImGui::SliderInt("Height", &height, 360, 2160)))
				App->window->SetSize(width, height);

			static bool fullscreen = App->window->fullscreen;
			static bool fullscreen_desktop = App->window->fullscreen_desktop;
			static bool resizable = App->window->resizable;
			static bool borderless = App->window->borderless;

			if (ImGui::Checkbox("Fullscreen", &fullscreen))
				App->window->SetFullscreen(fullscreen);

			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable))
				App->window->SetResizable(resizable);

			if (ImGui::Checkbox("Borderless", &borderless))
				App->window->SetBorderless(borderless);

			ImGui::SameLine();
			if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
				App->window->SetFullscreenDesktop(fullscreen_desktop);

			//Style
			const char* items[] = { "Classic", "Dark", "Light" };
			if (ImGui::Combo("Interface Style", &current_theme, items, IM_ARRAYSIZE(items)))
			{
				std::string theme = std::string(items[current_theme]);
				if (theme == "Dark") ImGui::StyleColorsDark();
				else if (theme == "Classic") ImGui::StyleColorsClassic();
				else if (theme == "Light") ImGui::StyleColorsLight();
			}
		}

		if (ImGui::CollapsingHeader("Renderer"))
		{
			static bool depth_test = glIsEnabled(GL_DEPTH_TEST);
			if (ImGui::Checkbox("Depth Test", &depth_test))
				App->renderer3D->SetCap(GL_DEPTH_TEST, depth_test);

			ImGui::SameLine();
			static bool cull_face = glIsEnabled(GL_CULL_FACE);
			if (ImGui::Checkbox("Cull Face", &cull_face))
				App->renderer3D->SetCap(GL_CULL_FACE, cull_face);

			static bool texture_2D = glIsEnabled(GL_TEXTURE_2D);
			if (ImGui::Checkbox("Texture 2D", &texture_2D))
				App->renderer3D->SetCap(GL_TEXTURE_2D, texture_2D);

			ImGui::SameLine();
			static bool lighting = glIsEnabled(GL_LIGHTING);
			if (ImGui::Checkbox("Lighting", &lighting))
				App->renderer3D->SetCap(GL_LIGHTING, lighting);


			static bool color_material = glIsEnabled(GL_COLOR_MATERIAL);
			if (ImGui::Checkbox("Color Material", &color_material))
				App->renderer3D->SetCap(GL_COLOR_MATERIAL, color_material);

			static bool vsync = App->renderer3D->vsync;
			if (ImGui::Checkbox("VSYNC", &vsync))
				App->renderer3D->SetVSync(vsync);

			if (ImGui::BeginMenu("Shading Mode"))
			{
				if (ImGui::MenuItem("Solid", NULL, App->renderer3D->display_mode == RenderMode::SOLID))
					App->renderer3D->SetDisplayMode(RenderMode::SOLID);
				if (ImGui::MenuItem("Wireframe", NULL, App->renderer3D->display_mode == RenderMode::WIREFRAME))
					App->renderer3D->SetDisplayMode(RenderMode::WIREFRAME);
				if (ImGui::MenuItem("Solid with wireframe", NULL, App->renderer3D->display_mode == RenderMode::SOLIDWIRE))
					App->renderer3D->SetDisplayMode(RenderMode::SOLIDWIRE);
				ImGui::EndMenu();
			}
		}

		if (ImGui::CollapsingHeader("Camera")) {
			static ImVec4 color = ImVec4(App->camera->background.r, App->camera->background.g, App->camera->background.b, App->camera->background.a);

			if (ImGui::ColorEdit3("Background Color", (float*)&color)) {
				App->camera->SetBackgroundColor(color.x, color.y, color.z, color.w);
			}

			ImGui::SliderFloat("Mouse Sensitivity", &App->camera->sensitivity, 0.0f, 50.0f);
			ImGui::SliderFloat("Movement Speed", &App->camera->movementSpeed, 0.0f, 50.0f);
			ImGui::SliderFloat("Orbit Speed", &App->camera->orbitingSpeed, 0.0f, 10.0f);
			ImGui::SliderFloat("Zoom Speed", &App->camera->zoomSpeed, 0.0f, 50.0f);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			const char* items[] = { "Vertical", "Horizontal" };
			if (ImGui::Combo("Fixed FOV", &cameraFov, items, IM_ARRAYSIZE(items)))
			{
				std::string currentFov = std::string(items[cameraFov]);
				if (currentFov == "Vertical") App->camera->SetFixedFOV(FOV::VERTICAL_FOV);
				else if (currentFov == "Horizontal") App->camera->SetFixedFOV(FOV::HORIZONTAL_FOV);
			}

			ImGui::Spacing();

			bool currentFixedFOV = App->camera->GetFixedFOV() == FOV::VERTICAL_FOV;

			if (currentFixedFOV)
			{
				float verticalFOV = App->camera->GetVerticalFieldOfView() * RADTODEG;
				if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
					App->camera->SetVerticalFieldOfView(verticalFOV * DEGTORAD, App->gui->sceneRenderSize.x, App->gui->sceneRenderSize.y);

				ImGui::Spacing();
				ImGui::Text("Horizontal FOV: %.2f", App->camera->GetHorizontalFieldOfView() * RADTODEG);
			}
			else
			{
				float horizontalFOV = App->camera->GetHorizontalFieldOfView() * RADTODEG;
				if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
					App->camera->SetHorizontalFieldOfView(horizontalFOV * DEGTORAD, App->gui->sceneRenderSize.x, App->gui->sceneRenderSize.y);

				ImGui::Spacing();
				ImGui::Text("Vertical FOV: %.2f", App->camera->GetVerticalFieldOfView() * RADTODEG);
			}
			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImVec4 values_color(1.0f, 1.0f, 0.0f, 1.0f);

			//SDL Version
			SDL_version version;
			SDL_GetVersion(&version);
			ImGui::Text("SDL Version:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%d.%d.%d", version.major, version.minor, version.patch);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//Hardware
			static Specs specs = App->GetSpecs();
			//CPU
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%d (Cache: %dkb)", specs.cpu_count, specs.cache);
			//RAM
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Gb", specs.ram);
			//Caps
			ImGui::Text("Caps:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%s", specs.caps.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//GPU
			ImGui::Text("GPU:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%s", specs.gpu);

			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%s", specs.gpu_brand);

			//VRAM
			GLint vram_budget, vram_usage, vram_available, vram_reserved;

			GetMemoryStatistics(specs.gpu_brand, vram_budget, vram_usage, vram_available, vram_reserved);

			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Mb", vram_budget * 0.001f);

			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Mb", vram_available * 0.001f);

		}

		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Mouse X: %d", App->input->GetMouseX());
			ImGui::Text("Mouse Y: %d", App->input->GetMouseY());

			ImGui::Spacing();

			ImGui::Text("Current Window Mouse X: %.2f", App->gui->mouseScenePosition.x);
			ImGui::Text("Current Window Mouse Y: %.2f", App->gui->mouseScenePosition.y);

			ImGui::Spacing();

			ImGui::Text("Normalized Mouse X: %.2f", App->gui->mouseScenePosition.x / App->gui->sceneRenderSize.x);
			ImGui::Text("Normalized Mouse Y: %.2f", App->gui->mouseScenePosition.y / App->gui->sceneRenderSize.y);

			ImGui::Spacing();

			float normalized_x = App->gui->mouseScenePosition.x / App->gui->sceneRenderSize.x;
			float normalized_y = App->gui->mouseScenePosition.y / App->gui->sceneRenderSize.y;

			normalized_x = (normalized_x - 0.5f) * 2.0f;
			normalized_y = -(normalized_y - 0.5f) * 2.0f;

			ImGui::Text("Near Plane Mouse X: %.2f", normalized_x);
			ImGui::Text("Near Plane Mouse Y: %.2f", normalized_y);
		}

		if (ImGui::CollapsingHeader("Time"))
		{
			ImGui::Text("Frame Count: %d", Time::frameCount);

			ImGui::Spacing();
			ImGui::Separator();

			ImGui::Text("Real Time");
			ImGui::Spacing();
			ImGui::Text("Delta Time: %.3f", Time::realClock.dt);
			ImGui::Text("Time Since Startup %.3f", Time::realClock.timeSinceStartup());

			ImGui::Spacing();
			ImGui::Separator();

			ImGui::Text("Game Time");
			ImGui::Spacing();
			ImGui::Text("Delta time %.3f", Time::gameClock.dt);
			ImGui::Text("Time Scale: %.2f", Time::gameClock.timeScale);
			ImGui::Text("Time since game start: %.2f", Time::gameClock.timeSinceStartup());
		}
	}
	ImGui::End();
}

void GuiConfiguration::GetMemoryStatistics(const char* gpu_brand, GLint& vram_budget, GLint& vram_usage, GLint& vram_available, GLint& vram_reserved)
{
	if (strcmp(gpu_brand, "NVIDIA Corporation") == 0)
	{
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram_usage);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_available);
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &vram_reserved);
	}
	else if (strcmp(gpu_brand, "ATI Technologies") == 0)
	{
		//glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		vram_budget = -1;
		//glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram_usage);
		vram_usage = -1;
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, &vram_available);
		//glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &vram_reserved);
		vram_reserved = -1;
	}
}