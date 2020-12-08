#include "Application.h"
#include "ModuleJson.h"
#include "Glew/include/glew.h"

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
	resources = new ModuleResources(true);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileManager);
	AddModule(window);
	AddModule(resources);
	AddModule(camera);
	AddModule(input);
	AddModule(resources);
	
	// Visual
	AddModule(scene);
	AddModule(gui);

	// Renderer last!
	AddModule(renderer3D);

	int cap = 60;
	capped_ms = 1000 / cap;
}

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

	LoadSpecs();

	fileManager->Init();
	time->Init();

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
	
	config.Release();
	RELEASE_ARRAY(buffer);

	time->realClock.deltaTimer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)time->realClock.deltaTimer.Read() / 1000.0f;
	last_FPS = 1.0f / dt;
	time->realClock.Step();
	time->gameClock.Step();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Uint32 last_frame_ms = time->realClock.deltaTimer.Read();
	if (last_frame_ms < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_ms);
	}

	time->frameCount++;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	if (wantToSave)
	{
		scene->Save(fileToSave);
		wantToSave = false;
	}
	else if (wantToLoad)
	{
		scene->Load(fileToLoad);
		wantToLoad = false;
	}

	if (!endFrameTasks.empty()) {
		for (size_t i = 0; i < endFrameTasks.size(); i++)
		{
			Module* mod = endFrameTasks.top();
			ret = mod->OnFrameEnd();
			endFrameTasks.pop();
		}
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (int i = list_modules.size() - 1; i > 0; i--)
	{
		list_modules[i]->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

float Application::GetMS()
{
	return time->realClock.deltaTimer.Read();
}

float Application::GetFPS()
{
	return last_FPS;
}

Specs Application::GetSpecs()
{
	return specs;
}

void Application::AddModuleToTaskStack(Module* callback)
{
	endFrameTasks.push(callback);
}

void Application::LoadSpecs()
{
	//CPU
	specs.cpu_count = SDL_GetCPUCount();
	specs.cache = SDL_GetCPUCacheLineSize();

	//RAM
	specs.ram = SDL_GetSystemRAM() / 1000;

	//GPU
	specs.gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	specs.gpu_brand = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

	//Caps
	specs.RDTSC = SDL_HasRDTSC();
	specs.MMX = SDL_HasMMX();
	specs.SSE = SDL_HasSSE();
	specs.SSE2 = SDL_HasSSE2();
	specs.SSE3 = SDL_HasSSE3();
	specs.SSE41 = SDL_HasSSE41();
	specs.SSE42 = SDL_HasSSE42();
	specs.AVX = SDL_HasAVX();
	specs.AVX2 = SDL_HasAVX2();
	specs.AltiVec = SDL_HasAltiVec();

	if (specs.RDTSC) specs.caps += "SDTSC";
	if (specs.MMX) specs.caps += ", MMX";
	if (specs.SSE) specs.caps += ", SSE";
	if (specs.SSE2) specs.caps += ", SSE2";
	if (specs.SSE3) specs.caps += ", SSE3";
	if (specs.SSE41) specs.caps += ", SSE41";
	if (specs.SSE42) specs.caps += ", SSE42";
	if (specs.AVX) specs.caps += ", AVX2";
	if (specs.AltiVec) specs.caps += ", AltiVec";
}