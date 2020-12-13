#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleJson.h"
#include "Component_Mesh.h"
#include "FileManager.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Component_Camera.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled), showGrid(true), selectedGameObject(nullptr), root(nullptr) 
{
	name = "scene";

	CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	CurrentGizmoMode = ImGuizmo::MODE::WORLD;
}

ModuleScene::~ModuleScene() 
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Create root gameObject
	root = new GameObject();
	root->SetName("Scene");
	selectedGameObject = root;
	
	//Create camera
	GameObject* camera = new GameObject();
	camera->AddComponent(ComponentType::CAMERA);
	camera->SetName("Camera");
	camera->GetTransform()->SetPosition(float3(0.0f, 0.0f, -5.0f));
	AddGameObject(camera);
	App->renderer3D->SetMainCamera(camera->GetComponent<Component_Camera>());

	//Create initial gameObject
	GameObject* street_environment = App->resources->RequestGameObject("Assets/Models/Street environment_V01.FBX");
	AddGameObject(street_environment);
	
	return ret;
}

// Clean up all gameObjects and scene
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");
	root->DeleteAllChildren();
	delete root;
	root = nullptr;
	selectedGameObject = nullptr;
	return true;
}

// Update: inputs and update gameobjects
update_status ModuleScene::Update(float dt)
{
	//Grid
	if (showGrid)
	{
		PrimitiveGrid grid(24);
		grid.Render();
	}

	//Inputs
	if ((App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) && (selectedGameObject != nullptr) && (selectedGameObject != root))
		selectedGameObject->toDelete = true;

	if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN))
		CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

	else if ((App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN))
		CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;

	else if ((App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN))
		CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;

	//Update GameObjects
	root->Update();

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

void ModuleScene::AddGameObject(GameObject* gameObject)
{
	if (gameObject != nullptr) 
	{
		root->AddChild(gameObject);
		selectedGameObject = gameObject;
	}
}

void ModuleScene::DeleteGameObject(GameObject* gameObject)
{
	if (root->RemoveChild(gameObject))
	{
		gameObject->DeleteAllChildren();
	}
	else if (gameObject->GetParent()->RemoveChild(gameObject))
	{
		gameObject->DeleteAllChildren();
	}

	delete gameObject;
	gameObject = nullptr;
}

std::vector<GameObject*> ModuleScene::GetAllGameObjects()
{
	std::vector<GameObject*> gameObjectsToGet;

	GetChildrenGameObjects(root, gameObjectsToGet);

	return gameObjectsToGet;
}

void ModuleScene::GetChildrenGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjectsToGet)
{
	gameObjectsToGet.push_back(gameObject);

	for (size_t i = 0; i < gameObject->GetChildrenAmount(); i++)
	{
		GetChildrenGameObjects(gameObject->GetChildAt(i), gameObjectsToGet);
	}
}

void ModuleScene::EditTransform()
{
	if (selectedGameObject != nullptr) 
	{
		float4x4 viewMatrix = App->camera->GetViewMatrixM().Transposed();
		float4x4 projectionMatrix = App->camera->GetProjectionMatrixM().Transposed();
		float4x4 objectTransform = selectedGameObject->GetTransform()->GetGlobalTransform().Transposed();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(App->gui->sceneWindowOrigin.x, App->gui->sceneWindowOrigin.y, App->gui->sceneRenderSize.x, App->gui->sceneRenderSize.y);

		float tempTransform[16];
		memcpy(tempTransform, objectTransform.ptr(), 16 * sizeof(float));

		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), CurrentGizmoOperation, CurrentGizmoMode, tempTransform);

		if (!(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (ImGuizmo::IsUsing()))
		{
			float4x4 newTransform;
			newTransform.Set(tempTransform);
			objectTransform = newTransform.Transposed();
			selectedGameObject->GetTransform()->SetGlobalTransform(objectTransform);
		}
	}
}

bool ModuleScene::ClearScene()
{
	bool ret = true;

	root->DeleteAllChildren();
	root = nullptr;

	return ret;
}

bool ModuleScene::Save(const char* file_path)
{
	bool ret = true;

	JsonObj saveFile;
	JsonArray gameObjects = saveFile.AddArray("Game Objects");

	root->Save(gameObjects);

	char* buffer = NULL;
	uint size = saveFile.Save(&buffer);

	if (FileManager::Save(file_path, buffer, size) == 0) ret = false;

	saveFile.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

bool ModuleScene::Load(const char* scene_file)
{
	bool ret = true;

	std::string extension = FileManager::GetFileExtension(scene_file);
	if (extension != ".scener")
	{
		LOG_WARNING("%s is not a valid scene extension and can't be loaded", scene_file);
		return false;
	}

	ClearScene();

	char* buffer = NULL;
	FileManager::Load(scene_file, &buffer);
	
	JsonObj base_object(buffer);
	JsonArray gameObjects(base_object.GetArray("Game Objects"));

	std::vector<GameObject*> createdObjects;

	for (size_t i = 0; i < gameObjects.Size(); i++)
	{
		//load game object
		GameObject* gameObject = new GameObject();
		uint parentUUID = gameObject->Load(&gameObjects.GetObjectAt(i));
		createdObjects.push_back(gameObject);

		//check if it's the root game object
		if (strcmp(gameObject->GetName(), "Scene") == 0) {
			root = gameObject;
			selectedGameObject = root;
		}

		//Get game object's parent
		for (size_t i = 0; i < createdObjects.size(); i++)
		{
			if (createdObjects[i]->UUID == parentUUID)
			{
				createdObjects[i]->AddChild(gameObject);
				gameObject->SetParent(createdObjects[i]);
			}
		}
	}

	root->UpdateChildrenTransforms();

	if (root != nullptr)
		LOG("Scene: %s loaded successfully", scene_file);

	return ret;
}

bool ModuleScene::LoadConfig(JsonObj& config)
{
	showGrid = config.GetBool("show_grid");

	return true;
}




