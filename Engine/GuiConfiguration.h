#ifndef __GUICONFIGURATION_H__
#define __GUICONFIGURATION_H__


#include "Module.h"
#include "Application.h"	
#include "Globals.h"
#include "GuiWindow.h"
#include "SDL/include/SDL.h"
#include "imGUI\imgui.h"

#include <vector>


class GuiConfiguration : public GuiWindow
{
public:
	GuiConfiguration();
	~GuiConfiguration();


	bool Start();
	void Draw();
	bool CleanUp();

public:

	SDL_version version;
	int cpu_cache = 0;
	int cpu_count = 0;
	int ram = 0;

	//3D Renderer
	bool depth_test = false;
	bool cull_face = false;
	bool lightning = false;
	bool color_material = false;
	bool texture2D = false;
	bool wireframe = false;


private:
	std::vector<float> vector_ms;
	std::vector<float> vector_fps;

};

#endif 