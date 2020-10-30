#include "Application.h"
#include "ModuleScene.h"
#include "Component_Texture.h"

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

	//Create root gameObject
	root = new GameObject("Root", nullptr, true);
	root->id = 0;
	currentID = 0;
	gameObjects.push_back(root);

	App->camera->Position = vec3(0, 1, 4);
	App->camera->LookAt(vec3(0, 0, 0));

	App->importer->LoadMesh("Assets/baker_house/BakerHouse.fbx");
	uint bhTexture = App->importer->LoadTexture("Assets/baker_house/Baker_house.png");
	Component_Texture* bhTex = (Component_Texture*)GetGameObject("Baker_house")->CreateComponent(Component::COMPONENT_TYPE::TEXTURE);
	bhTex->SetIdTexture(bhTexture);
	bhTex = (Component_Texture*)GetGameObject("Chimney")->CreateComponent(Component::COMPONENT_TYPE::TEXTURE);
	bhTex->SetIdTexture(bhTexture);

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
	//Update all gameObjects
	std::vector<GameObject*>::iterator currentGO = gameObjects.begin();
	
	for (; currentGO != gameObjects.end(); currentGO++) {
		(*currentGO)->Update();
	}


	//Inputs
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

	//Create component transform
	newGameObject->CreateComponent(Component::COMPONENT_TYPE::TRANSFORM);

	parent->AddGameObjectAsChild(newGameObject);
	currentID++;
	newGameObject->id = currentID;
	gameObjects.push_back(newGameObject);

	//Log gameObject creation
	std::string logText = "Created new GameObject called ";
	logText += name;
	LOG(logText.c_str());

	return newGameObject;
}

//Get gameObject functions
GameObject* ModuleScene::GetGameObject(const char* name) //This method is slower but more precise
{
	std::vector<GameObject*>::iterator currentGO = gameObjects.begin();

	for (; currentGO != gameObjects.end(); currentGO++) {
		if (strcmp((*currentGO)->GetName(), name) == 0) {
			return (*currentGO);
		}
	}
	//Log gameObject searh error
	std::string logText = "Not found gameObject with name: ";
	logText += name;
	LOG(logText.c_str());
	return nullptr;
}

GameObject* ModuleScene::GetGameObject(int id) //This method is faster but it could fail
{
	GameObject* ret;
	ret = gameObjects.at(id);

	if (ret != nullptr) {
		return ret;
	}
	else {
		//Log gameObject searh error
		std::string logText = "Not found gameObject with id: ";
		logText += id;
		LOG(logText.c_str());
		return nullptr;
	}
}