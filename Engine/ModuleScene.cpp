#include "Application.h"
#include "ModuleScene.h"

#include "ImGui/imgui.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	root = new GameObject("Root", nullptr, true);

	App->camera->Position = vec3(0, 1, 4);
	App->camera->LookAt(vec3(0, 0, 0));

	App->importer->LoadMesh("Assets/baker_house/BakerHouse.fbx");

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	std::vector<GameObject*>::iterator currentGO = gameObjects.begin();
	
	for (; currentGO != gameObjects.end(); currentGO++) {
		(*currentGO)->Update();
	}

	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) 
	{
		App->camera->Position = vec3(0, 1, 4);
		App->camera->LookAt(vec3(0, 0, 0));
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		LOG("Console test");
	}

	//Grid
	Grid grid(20);
	grid.Render();

	//Rendering primitives
	//uncomment the primitive u want to see
	//Cube cube;
	//cube.Render();
	//Plane plane;
	//plane.Render();
	//Pyramid pyramid;
	//pyramid.Render();

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent, bool enabled) 
{
	if (parent == nullptr) parent = root;

	GameObject* newGameObject = new GameObject(name, parent, enabled);

	//create component transform

	parent->AddGameObjectAsChild(newGameObject);

	gameObjects.push_back(newGameObject);

	return newGameObject;
}