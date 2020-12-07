#include "Application.h"
#include "ModuleJson.h"

Application::Application(int argc, char* args[]) : argc(argc), args(args), wantToSave(false), wantToLoad(false), inGame(false)
{
	fileManager = new FileManager(true);
	time = new Time();

	window = new ModuleWindow(true);
	input = new ModuleInput(true);
	renderer3D = new ModuleRenderer3D(true);
	camera = new ModuleCamera3D(true);
	scene = new ModuleScene(true);
	gui = new ModuleGui(true);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileManager);
	AddModule(window);
	AddModule(resources);
	AddModule(camera);
	AddModule(input);
	
	// Visual
	AddModule(scene);
	AddModule(gui);

	// Renderer last!
	AddModule(renderer3D);
}

Application* App = nullptr;

Application::~Application()
{
	std::vector<Module*>::reverse_iterator item = list_modules.rbegin();

	while (item != list_modules.rend())
	{
		delete* item;
		++item;
	}
	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	fileManager->Init();

	char* buffer = nullptr;

	uint size = fileManager->Load("Library/Config/config.json", &buffer);
	JsonObj config(buffer);

	engineName = config.GetString("engineName", TITLE);
	engineVersion = config.GetString("version", "0.0");

	JsonArray modules_array(config.GetArray("modules_config"));

	// Call Init() in all modules
	for (int i = 0; i < list_modules.size() && ret == true; i++)
	{
		JsonObj module_config(modules_array.GetObjectInArray(list_modules[i]->name));

		ret = list_modules[i]->LoadConfig(module_config);
		ret = list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	last_FPS = 1.0f / dt;
	last_ms = (float)ms_timer.Read();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item;
	item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		item++;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::iterator item;
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

float Application::GetMS()
{
	return last_ms;
}

float Application::GetFPS()
{
	return last_FPS;
}