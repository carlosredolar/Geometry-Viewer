#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
//#include "Glew/include/glew.h"
#include "GameObject.h"

#include "Libs/ImGui/imgui.h"
#include "Libs/ImGuizmo/ImGuizmo.h"

#include <vector>

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	bool LoadConfig(JsonObj& config) override;
	update_status Update(float dt);
	bool CleanUp();
	GameObject* CreateGameObject(const char* name, GameObject* parent, bool enabled);
	GameObject* GetGameObject(const char* name);
	GameObject* GetGameObject(int UUID);
	std::vector<GameObject*> GetAllGameObjects();

	void SelectGameObject(GameObject* selected);
	GameObject* GetSelectedGameObject();

	bool DeleteGameObject(GameObject* todelete);
	GameObject* GetRoot() { return root; }

	bool ClearScene();

	bool Save(const char* file_path);
	bool Load(const char* scene_file);
private:

	int currentID;
	GameObject* root;
	std::vector<GameObject*> gameObjects;
	std::vector<int*> goID;

	ImGuizmo::OPERATION mCurrentGizmoOperation;
	ImGuizmo::MODE mCurrentGizmoMode;

	bool show_grid = true;
};
