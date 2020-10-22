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

	App->camera->Position = vec3(0, 1, 4);
	App->camera->LookAt(vec3(0, 0, 0));

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
	Plane2 p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) 
	{
		App->camera->Position = vec3(0, 1, 4);
		App->camera->LookAt(vec3(0, 0, 0));
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		LOG("Console test");
	}

	//Rendering primitives
	Cube2 cube;
	cube.Render();
	//Plane3 plane;
	//plane.Render();
	//Pyramid2 pyramid;
	//pyramid.Render();
	//Sphere2 sphere;
	//sphere.Render();
	//Cylinder2 cylinder;
	//cylinder.Render();

	

	/*static float vertex[] = {
						1.0f, 1.0f, 1.0f, //v0 
						-1.0f, 1.0f, 1.0f, //v1
						-1.0f, -1.0f, 1.0f, //v2
						1.0f, -1.0f, 1.0f, //v3
						1.0f, -1.0f, -1.0f, //v4
						1.0f, 1.0f, -1.0f, //v5
						-1.0f, 1.0f, -1.0f, //v6
						-1.0f, -1.0f, -1.0f //v7
	};
	static uint indices[] = { //Front
							  0, 1, 2,
							  2, 3, 0,
							  //Right
							  0, 3, 4,
							  4, 5, 0,
							  //Up
							  0, 5, 6,
							  6, 1, 0,
							  //Back
							  7, 6, 5,
							  5, 4, 7,
							  //Left
							  7, 2, 1,
							  1, 6, 7,
							  //Back
							  7, 4, 3,
							  3, 2, 7
	};
	
	//Vertex
	uint vert_id = 0;
	glGenBuffers(1, (GLuint*)& (vert_id));
	glBindBuffer(GL_ARRAY_BUFFER, vert_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Index
	uint ind_id = 0;
	glGenBuffers(1, (GLuint*)& (ind_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//Draw
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, vert_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_id);
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glDisableClientState(GL_VERTEX_ARRAY);*/

	return UPDATE_CONTINUE;
}

