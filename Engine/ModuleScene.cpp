#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleJson.h"
#include "Component_Mesh.h"
#include "FileManager.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Component_Image.h"
#include "Component_Camera.h"
#include "Component_Graphic.h"
#include "ModuleCamera3D.h"

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
	Component_Camera* cam = (Component_Camera*)camera->AddComponent(ComponentType::CAMERA);
	camera->SetName("Camera");
	camera->GetTransform()->SetPosition(cam->GetFrustum().pos);
	AddGameObject(camera);
	App->renderer3D->SetMainCamera(cam);

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
	if (App->in_game)
	{
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			GameObject* can = FindGameObjectWithName("Canvas 1");
			can->Enable(!can->IsEnabled());
		}
	}
	else
	{
		//Grid
		if (showGrid)
		{
			PrimitiveGrid grid(24);
			grid.Render();
		}
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

	for (uint i = 0; i < gameObject->GetChildrenAmount(); i++)
	{
		GetChildrenGameObjects(gameObject->GetChildAt(i), gameObjectsToGet);
	}
}

GameObject* ModuleScene::FindGameObjectWithName(const char* gameObjectName)
{
	std::vector<GameObject*> gameObjects = GetAllGameObjects();
	for (uint i = 0; i < gameObjects.size(); i++)
	{
		if (strcmp(gameObjects.at(i)->GetName(), gameObjectName) == 0)
		{
			return gameObjects.at(i);
		}
	}
	return nullptr;
}

void ModuleScene::EditTransform()
{
	if (selectedGameObject != nullptr) 
	{
		float4x4 viewMatrix = App->camera->GetViewMatrixM().Transposed();
		float4x4 projectionMatrix = App->camera->GetProjectionMatrixM().Transposed();
		float4x4 objectTransform = selectedGameObject->GetTransform()->GetGlobalTransform();

		float tempTransform[16];

		objectTransform = objectTransform.Transposed();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(App->gui->sceneWindowOrigin.x, App->gui->sceneWindowOrigin.y, App->gui->sceneRenderSize.x, App->gui->sceneRenderSize.y);

		memcpy(tempTransform, objectTransform.ptr(), 16 * sizeof(float));

		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), CurrentGizmoOperation, CurrentGizmoMode, tempTransform);

		if (!(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (ImGuizmo::IsUsing()))
		{
			float4x4 newTransform;
			newTransform.Set(tempTransform);
			objectTransform = newTransform.Transposed();
			selectedGameObject->GetTransform()->SetGlobalTransform(objectTransform);
			movingObject = true;
		}

		if (movingObject && (App->input->GetMouseButton(1) == KEY_UP))
		{
			if (selectedGameObject->GetComponent<Component_Graphic>() != nullptr)
				selectedGameObject->SetAABB(selectedGameObject->GetComponent<Component_Graphic>()->GetAABB());
			movingObject = false;
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
	saveFile.AddFloat3("Camera Position", App->camera->GetPosition());
	saveFile.AddFloat3("Camera Reference", App->camera->GetReferencePos());

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

	App->camera->SetPosition(base_object.GetFloat3("Camera Position"));
	App->camera->SetReferencePos(base_object.GetFloat3("Camera Reference"));
	App->gui->sceneWindowFocused = true;

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




