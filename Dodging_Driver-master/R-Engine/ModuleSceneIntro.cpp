#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));


	//MAP LIMITS

	//right wall
	CreateBox(0, { 0, 1, 0 }, { -20, 1, MAP_LENGHT/2 }, { 1,80,MAP_LENGHT +20}, Sky);
	//left wall
	CreateBox(0, { 0, 1, 0 }, { 20, 1, MAP_LENGHT/2 }, { 1,80,MAP_LENGHT +20}, Sky);
	//floor wall
	CreateBox(0, { 0, 1, 0 }, { 0, 1, MAP_LENGHT / 2 }, { 40,1,MAP_LENGHT +20}, Grass);
	//back start wall
	CreateBox(0, { 0, 1, 0 }, { 0, 1, -10}, { 40,80,1 }, Sky);
	//end wall
	CreateBox(0, { 0, 1, 0 }, { 0, 1, MAP_LENGHT }, { 40,80,1 }, Green);
	//.......................................................................

	//MAP OBSTACLES

	//ramps
	CreateBox(-10, { 1, 0, 0 }, { 0, 1, 100 }, { 40,1,20 }, Gray);
	CreateBox(-30, { 1, 0, 0 }, { 0, 1, 950 }, { 40,1,20 }, Gray);
	//trees
	for (float i = 0; i < 7; i++)
	{
		float distance = i * 100;
		CreateBox(0, { 0, 1, 0 }, { 10, 3, distance }, { 1,10,1 }, Brown);

		CreateBox(0, { 0, 1, 0 }, { -5, 3, distance +40 }, { 1,10,1 }, Brown);

		CreateBox(0, { 0, 1, 0 }, { -5, 3, distance +90 }, { 1,10,1 }, Brown);

		CreateBox(0, { 0, 1, 0 }, { 5, 3, distance +120 }, { 1,10,1 }, Brown);

		CreateBox(0, { 0, 1, 0 }, { -15, 3, distance +110 }, { 1,10,1 }, Brown);

		CreateBox(0, { 0, 1, 0 }, { 15, 3, distance + 160 }, { 1,10,1 }, Brown);

		CreateBox(0, { 0, 1, 0 }, { 0, 3, distance + 205 }, { 1,10,1 }, Brown);
	}
	//people
	CreateBox(20, { 0, 0, 1 }, { 10, 3, 50 }, { 1,4,1 }, Black);
	CreateBox(-20, { 0, 0, 1 }, { 8, 3, 50 }, { 1,4,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { 9, 7, 50 }, { 1,5,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { 9, 9.5, 50 }, { 2,2,2 }, Black);
	CreateBox(0, { 0, 0, 1 }, { 9, 7.5, 50 }, { 4,1,1 }, Black);

	CreateBox(20, { 0, 0, 1 }, { -10, 3, 200 }, { 1,4,1 }, Black);
	CreateBox(-20, { 0, 0, 1 }, { -12, 3, 200 }, { 1,4,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -11, 7, 200 }, { 1,5,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -11, 9.5, 200 }, { 2,2,2 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -11, 7.5, 200 }, { 4,1,1 }, Black);

	CreateBox(20, { 0, 0, 1 }, { 5, 3, 450 }, { 1,4,1 }, Black);
	CreateBox(-20, { 0, 0, 1 }, { 3, 3, 450 }, { 1,4,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { 4, 7, 450 }, { 1,5,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { 4, 9.5, 450 }, { 2,2,2 }, Black);
	CreateBox(0, { 0, 0, 1 }, { 4, 7.5, 450 }, { 4,1,1 }, Black);

	CreateBox(20, { 0, 0, 1 }, { -7, 3, 700 }, { 1,4,1 }, Black);
	CreateBox(-20, { 0, 0, 1 }, { -9, 3, 700 }, { 1,4,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -8, 7, 700 }, { 1,5,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -8, 9.5, 700 }, { 2,2,2 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -8, 7.5, 700 }, { 4,1,1 }, Black);


	//WIN letters
	//W
	CreateBox(-20, { 0, 0, 1 }, { 15, 20, MAP_LENGHT - 3 }, { 1,10,1 }, Black);
	CreateBox(20, { 0, 0, 1 }, { 5, 20, MAP_LENGHT - 3 }, { 1,10,1 }, Black);
	CreateBox(-30, { 0, 0, 1 }, { 8.5, 18, MAP_LENGHT - 3 }, { 1,6,1 }, Black);
	CreateBox(30, { 0, 0, 1 }, { 11.5, 18, MAP_LENGHT - 3 }, { 1,6,1 }, Black);
	//I
	CreateBox(0, { 0, 0, 1 }, { 0, 20, MAP_LENGHT - 3 }, { 1,9,1 }, Black);
	//N
	CreateBox(0, { 0, 0, 1 }, { -7, 20, MAP_LENGHT - 3 }, { 1,9,1 }, Black);
	CreateBox(0, { 0, 0, 1 }, { -13, 20, MAP_LENGHT - 3 }, { 1,9,1 }, Black);
	CreateBox(-30, { 0, 0, 1 }, { -10, 20, MAP_LENGHT - 3 }, { 1,10,1 }, Black);


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Plane p(0, 1, 0, 0);
	//p.axis = true;
	//p.Render();

	//MAP LIMITS

	//right wall
	RenderBox(0, { 0, 1, 0 }, { -20, 1, MAP_LENGHT / 2 }, { 1,80,MAP_LENGHT +20}, Sky);
	//left wall
	RenderBox(0, { 0, 1, 0 }, { 20, 1, MAP_LENGHT / 2 }, { 1,80,MAP_LENGHT +20}, Sky);
	//floor wall
	RenderBox(0, { 0, 1, 0 }, { 0, 1, MAP_LENGHT / 2 }, { 40,1,MAP_LENGHT +20}, Grass);
	//back start wall
	RenderBox(0, { 0, 1, 0 }, { 0, 1, -10}, { 40,80,1 }, Sky);
	//end wall
	RenderBox(0, { 0, 1, 0 }, { 0, 1, MAP_LENGHT }, { 40,80,1 }, Green);
	//.......................................................................

	//MAP OBSTACLES

	//ramps
	RenderBox(-10, { 1, 0, 0 }, { 0, 1, 100 }, { 40,1,20 }, Gray);
	RenderBox(-30, { 1, 0, 0 }, { 0, 1, 950 }, { 40,1,20 }, Gray);
	//trees
	for (float i = 0; i < 7; i++)
	{
		float distance = i * 100;
		RenderBox(0, { 0, 1, 0 }, { 10, 3, distance }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { 10, 9, distance }, { 6,6,6 }, Deep_Green);

		RenderBox(0, { 0, 1, 0 }, { -5, 3, distance + 40 }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { -5, 9, distance + 40 }, { 6,6,6 }, Deep_Green);

		RenderBox(0, { 0, 1, 0 }, { -5, 3, distance + 90 }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { -5, 9, distance + 90 }, { 6,6,6 }, Deep_Green);

		RenderBox(0, { 0, 1, 0 }, { 5, 3, distance + 120 }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { 5, 9, distance + 120 }, { 6,6,6 }, Deep_Green);

		RenderBox(0, { 0, 1, 0 }, { -15, 3, distance + 110 }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { -15, 9, distance + 110 }, { 6,6,6 }, Deep_Green);

		RenderBox(0, { 0, 1, 0 }, { 15, 3, distance + 160 }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { 15, 9, distance + 160 }, { 6,6,6 }, Deep_Green);

		RenderBox(0, { 0, 1, 0 }, { 0, 3, distance + 205 }, { 1,10,1 }, Brown);
		RenderBox(0, { 0, 1, 0 }, { 0, 9, distance + 205 }, { 6,6,6 }, Deep_Green);
	}
	//people
	RenderBox(20, { 0, 0, 1 }, { 10, 3, 50 }, { 1,4,1 }, Black);
	RenderBox(-20, { 0, 0, 1 }, { 8, 3, 50 }, { 1,4,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { 9, 7, 50 }, { 1,5,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { 9, 9.5, 50 }, { 2,2,2 }, Black);
	RenderBox(0, { 0, 0, 1 }, { 9, 7.5, 50 }, { 4,1,1 }, Black);

	RenderBox(20, { 0, 0, 1 }, { -10, 3, 200 }, { 1,4,1 }, Black);
	RenderBox(-20, { 0, 0, 1 }, { -12, 3, 200 }, { 1,4,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -11, 7, 200 }, { 1,5,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -11, 9.5, 200 }, { 2,2,2 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -11, 7.5, 200 }, { 4,1,1 }, Black);

	RenderBox(20, { 0, 0, 1 }, { 5, 3, 450 }, { 1,4,1 }, Black);
	RenderBox(-20, { 0, 0, 1 }, { 3, 3, 450 }, { 1,4,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { 4, 7, 450 }, { 1,5,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { 4, 9.5, 450 }, { 2,2,2 }, Black);
	RenderBox(0, { 0, 0, 1 }, { 4, 7.5, 450 }, { 4,1,1 }, Black);

	RenderBox(20, { 0, 0, 1 }, { -7, 3, 700 }, { 1,4,1 }, Black);
	RenderBox(-20, { 0, 0, 1 }, { -9, 3, 700 }, { 1,4,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -8, 7, 700 }, { 1,5,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -8, 9.5, 700 }, { 2,2,2 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -8, 7.5, 700 }, { 4,1,1 }, Black);

	//WIN letters
	//W
	RenderBox(-20, { 0, 0, 1 }, { 15, 20, MAP_LENGHT-3 }, { 1,10,1 }, Black);
	RenderBox(20, { 0, 0, 1 }, { 5, 20, MAP_LENGHT - 3 }, { 1,10,1 }, Black);
	RenderBox(-30, { 0, 0, 1 }, { 8.5, 18, MAP_LENGHT - 3 }, { 1,6,1 }, Black);
	RenderBox(30, { 0, 0, 1 }, { 11.5, 18, MAP_LENGHT - 3 }, { 1,6,1 }, Black);
	//I
	RenderBox(0, { 0, 0, 1 }, { 0, 20, MAP_LENGHT - 3 }, { 1,9,1 }, Black);
	//N
	RenderBox(0, { 0, 0, 1 }, { -7, 20, MAP_LENGHT - 3 }, { 1,9,1 }, Black);
	RenderBox(0, { 0, 0, 1 }, { -13, 20, MAP_LENGHT - 3 }, { 1,9,1 }, Black);
	RenderBox(-30, { 0, 0, 1 }, { -10, 20, MAP_LENGHT - 3 }, { 1,10,1 }, Black);


	if (SDL_GetTicks() >= TIME_TRIAL)
	{
		return UPDATE_STOP;
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

void ModuleSceneIntro::CreateBox(int degrees, vec3 angle, vec3 offset, vec3 size, Color color) {

	Cube cube;
	cube.size = size;
	cube.SetPos(offset.x, offset.y, offset.z);
	cube.color = color;
	cube.SetRotation(degrees, angle);
	App->physics->AddBody(cube, 0);
	cube.Render();
}

void ModuleSceneIntro::RenderBox(int degrees, vec3 angle, vec3 offset, vec3 size, Color color) {

	Cube cube;
	cube.size = size;
	cube.SetPos(offset.x, offset.y, offset.z);
	cube.color = color;
	cube.SetRotation(degrees, angle);
	cube.Render();
}
