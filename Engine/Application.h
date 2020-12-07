#pragma once
#include "Globals.h"
#include "Time.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "ModuleGui.h"
#include "ModuleImport.h"
#include "FileManager.h"
#include "ModuleResources.h"

#include <stack>

struct Specs
{
	int cpu_count;
	int cache;
	float ram;
	const char* gpu;
	const char* gpu_brand;
	float vram_budget;
	float vram_usage;
	float vram_available;
	float vram_reserved;

	bool RDTSC = false;
	bool MMX = false;
	bool SSE = false;
	bool SSE2 = false;
	bool SSE3 = false;
	bool SSE41 = false;
	bool SSE42 = false;
	bool AVX = false;
	bool AVX2 = false;
	bool AltiVec = false;

	std::string caps;
};

class Application
{
public:
	FileManager* fileManager;
	Time* time;

	ModuleWindow* window;
	ModuleInput* input;
	ModuleCamera3D* camera;
	ModuleScene* scene;
	ModuleGui* gui;
	ModuleRenderer3D* renderer3D;
	ModuleResources* resources;

	bool inGame;
	const char* engineName;
	const char* engineVersion;

private:
	int	   argc;
	char** args;

	float	dt = 0;
	float last_FPS = 0.0f;
	float capped_ms;

	const char* config_path;

	bool wantToSave;
	bool wantToLoad;

	const char* fileToLoad;
	const char* fileToSave;

	std::vector<Module*> list_modules;

	std::stack<Module*> endFrameTasks;

public:

	Application(int argc, char* args[]);
	~Application();

	void StartGame();
	void StopGame();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetMS();
	float GetFPS();

	void SetFPSCap(int fps_cap);
	void Save(const char* filePath);
	void Load(const char* filePath);

	void AddModuleToTaskStack(Module* callback);
	Specs GetSpecs();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	void LoadSpecs();
protected:
	Specs specs;
};
extern Application* App;