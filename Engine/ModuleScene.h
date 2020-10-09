#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "glew.h"

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
