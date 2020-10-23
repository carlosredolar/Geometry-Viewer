#pragma once
#include "Globals.h"
#include "Module.h"

struct mesh
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	bool enabled = true;
};

class ModuleImport : public Module
{
public:
	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	bool Init();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void LoadMesh(char* file_path);
	void RenderMesh(mesh* m);
	std::vector<mesh> meshes;
};