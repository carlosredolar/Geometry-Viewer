#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "ModuleImport.h"
#include "glew.h"
#include "GameObject.h"

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	GameObject* CreateGameObject(const char* name, GameObject* parent, bool enabled);
	GameObject* GetGameObject(const char* name);
	GameObject* GetGameObject(int id);
	void SelectGameObject(GameObject* selected);

private:

	GameObject* root;
	std::vector<GameObject*> gameObjects;
	std::vector<int*> goID;
	int currentID;

};
