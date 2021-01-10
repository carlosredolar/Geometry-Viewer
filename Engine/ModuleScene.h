#pragma once
#include "Module.h"
#include "Globals.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/ImGuizmo/ImGuizmo.h"

#include <vector>

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init() { return true; };

	bool Start();
	bool LoadConfig(JsonObj& config) override;
	update_status Update(float dt);
	bool CleanUp();

	void AddGameObject(GameObject* gameObject);
	void DeleteGameObject(GameObject* gameObject);
	std::vector<GameObject*> GetAllGameObjects();
	void GetChildrenGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects);
	void EditTransform();

	GameObject* FindGameObjectWithName(const char* gameObjectName);

	GameObject* GetRoot() { return root; }
	bool ClearScene();

	bool Save(const char* file_path);
	bool Load(const char* scene_file);

public:
	bool showGrid;
	bool showBB;

	GameObject* selectedGameObject;

private:
	GameObject* root;

	bool movingObject = false;

	ImGuizmo::OPERATION CurrentGizmoOperation;
	ImGuizmo::MODE CurrentGizmoMode;
};
