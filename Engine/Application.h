#pragma once

#include "Globals.h"
#include "Timer.h"
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


	Timer	ms_timer;
	float	dt = 0;
	float last_FPS = 0.0f;
	float last_ms = 0.0f;

	const char* config_path;

	bool wantToSave;
	bool wantToLoad;

	const char* _file_to_load;
	const char* _file_to_save;

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

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};
extern Application* App;