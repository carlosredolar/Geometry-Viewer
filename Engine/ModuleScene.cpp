#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "Component_Material.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
	name = "scene";
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Create root gameObject
	root = new GameObject("Scene", nullptr, true);
	root->CreateComponent(ComponentType::TRANSFORM);
	root->UUID = 0;

	currentID = 0;
	gameObjects.push_back(root);

	App->camera->SetPosition(float3(0, 3, 10));
	App->camera->LookAt(float3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Scene");

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
		App->camera->SetPosition(float3(0, 1, 4));
		App->camera->LookAt(float3(0, 0, 0));
	}

	//if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) 
	//{
	//	if (GetSelectedGameObject() != nullptr) 
	//	{
	//		GameObject* parent = GetSelectedGameObject()->GetParent();
	//		DeleteGameObject(GetSelectedGameObject());
	//		SelectGameObject(parent);
	//	}
	//}

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
	newGameObject->CreateComponent(ComponentType::TRANSFORM);

	parent->AddGameObjectAsChild(newGameObject);
	currentID++;
	newGameObject->UUID = currentID;
	gameObjects.push_back(newGameObject);

	return newGameObject;
}

//Get gameObject functions
GameObject* ModuleScene::GetGameObject(const char* name) //This method could fail if 2 gameObjects are named the same
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

GameObject* ModuleScene::GetGameObject(int UUID) //This method is fast and precise
{
	GameObject* ret;
	ret = gameObjects.at(UUID);

	if (ret != nullptr) {
		return ret;
	}
	else {
		//Log gameObject searh error
		std::string logText = "Not found gameObject with id: ";
		logText += UUID;
		LOG(logText.c_str());
		return nullptr;
	}
}

void ModuleScene::SelectGameObject(GameObject* selected) 
{
	std::vector<GameObject*>::iterator currentGO = gameObjects.begin();

	for (; currentGO != gameObjects.end(); currentGO++) {
		if ((*currentGO) == selected)
		{
			(*currentGO)->selected = true;
			App->gui->SelectGameObject((*currentGO));
		}
		else (*currentGO)->selected = false;
	}
}

GameObject* ModuleScene::GetSelectedGameObject()
{
	std::vector<GameObject*>::iterator currentGO = gameObjects.begin();

	for (; currentGO != gameObjects.end(); currentGO++) {
		if ((*currentGO)->selected)
		{
			return (*currentGO);
		}
	}

	return nullptr;
}

std::vector<GameObject*> ModuleScene::GetAllGameObjects()
{
	return gameObjects;
}

bool ModuleScene::DeleteGameObject(GameObject* todelete)
{
	//if (todelete->GetChilds()->size() > 0) 
	//{
	//	std::vector<GameObject*>::iterator currentGO = todelete->GetChilds()->begin();

	//	for (; currentGO != gameObjects.end(); currentGO++) {
	//		DeleteGameObject((*currentGO));
	//	}
	//}
	todelete->DeleteComponents();

	delete(todelete);
	return true;
}

bool ModuleScene::ClearScene()
{
	bool ret = true;

	root->DeleteAllChilds();
	root = nullptr;

	return ret;
}

bool ModuleScene::Save(const char* file_path)
{
	bool ret = true;

	JsonObj save_file;

	JsonArray gameObjects = save_file.AddArray("Game Objects");

	root->Save(gameObjects);

	char* buffer = NULL;
	uint size = save_file.Save(&buffer);

	App->fileManager->Save(file_path, buffer, size);

	save_file.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

bool ModuleScene::Load(const char* scene_file)
{
	bool ret = true;

	std::string extension = App->fileManager->ExtractFileExtension(scene_file);
	if (extension != ".scene")
	{
		WARNING_LOG("%s is not a valid scene extension and can't be loaded", scene_file);
		return false;
	}

	ClearScene();

	char* buffer = NULL;
	App->fileManager->Load(scene_file, &buffer);

	JsonObj base_object(buffer);
	JsonArray loadedGameObjects(base_object.GetArray("Game Objects"));

	std::vector<GameObject*> createdObjects;

	for (size_t i = 0; i < loadedGameObjects.Size(); i++)
	{
		//load game object
		GameObject* gameObject = new GameObject(loadedGameObjects.GetObjectAt(i).GetString("Name", "GameObject"), App->scene->GetRoot());
		uint parentUUID = gameObject->Load(&loadedGameObjects.GetObjectAt(i));
		createdObjects.push_back(gameObject);

		//check if it's the root game object
		if (strcmp(gameObject->GetName(), "Root") == 0) {
			root = gameObject;
			SelectGameObject(root);
		}

		//Get game object's parent
		for (size_t i = 0; i < createdObjects.size(); i++)
		{
			if (createdObjects[i]->UUID == parentUUID)
			{
				createdObjects[i]->AddGameObjectAsChild(gameObject);
				gameObject->ChangeParent(createdObjects[i]);
			}
		}
	}

	//root->UpdateChildrenTransforms();

	if (root != nullptr)
		LOG("Scene: %s loaded successfully", scene_file);

	return ret;
}

bool ModuleScene::LoadConfig(JsonObj & config)
{
	show_grid = config.GetBool("show_grid");

	return true;
}
