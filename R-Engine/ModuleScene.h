#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2
#define MAP_LENGHT 1000
#define TIME_TRIAL 50*1000

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void CreateBox(int degrees, vec3 angle, vec3 offset, vec3 size, Color color);
	void RenderBox(int degrees, vec3 angle, vec3 offset, vec3 size, Color color);

public:

};
