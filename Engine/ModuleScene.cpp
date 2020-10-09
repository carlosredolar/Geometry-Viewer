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

	/*glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(1.f, 0.f, 0.f);
		glVertex3f(0.f, 1.f, 0.f);

		glVertex3f(1.f, 0.f, 0.f);
		glVertex3f(1.f, 1.f, 0.f);
		glVertex3f(0.f, 1.f, 0.f);

		glVertex3f(0.f, 0.f, -1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 1.f, 0.f);

		glVertex3f(0.f, 0.f, -1.f);
		glVertex3f(0.f, 1.f, 0.f);
		glVertex3f(0.f, 1.f, -1.f);

		glVertex3f(1.f, 0.f, 0.f);
		glVertex3f(1.f, 0.f, -1.f);
		glVertex3f(1.f, 1.f, -1.f);

		glVertex3f(1.f, 0.f, 0.f);
		glVertex3f(1.f, 1.f, -1.f);
		glVertex3f(1.f, 1.f, 0.f);

		glVertex3f(1.f, 0.f, -1.f);
		glVertex3f(0.f, 0.f, -1.f);
		glVertex3f(0.f, 1.f, -1.f);

		glVertex3f(1.f, 0.f, -1.f);
		glVertex3f(0.f, 1.f, -1.f);
		glVertex3f(1.f, 1.f, -1.f);
	glEnd();
	glLineWidth(1.0f);*/

	//const int shapesize = 108;

	//float shape[shapesize] =
	//{
	//	0.f, 0.f, 0.f,
	//	1.f, 0.f, 0.f,
	//	0.f, 1.f, 0.f,

	//	1.f, 0.f, 0.f,
	//	1.f, 1.f, 0.f,
	//	0.f, 1.f, 0.f,

	//	0.f, 0.f, -1.f,
	//	0.f, 0.f, 0.f,
	//	0.f, 1.f, 0.f,

	//	0.f, 0.f, -1.f,
	//	0.f, 1.f, 0.f,
	//	0.f, 1.f, -1.f,

	//	1.f, 0.f, 0.f,
	//	1.f, 0.f, -1.f,
	//	1.f, 1.f, -1.f,

	//	1.f, 0.f, 0.f,
	//	1.f, 1.f, -1.f,
	//	1.f, 1.f, 0.f,

	//	1.f, 0.f, -1.f,
	//	0.f, 0.f, -1.f,
	//	0.f, 1.f, -1.f,

	//	1.f, 0.f, -1.f,
	//	0.f, 1.f, -1.f,
	//	1.f, 1.f, -1.f,

	//	0.f, 1.f, 0.f,
	//	1.f, 1.f, 0.f,
	//	1.f, 1.f, -1.f,

	//	0.f, 1.f, 0.f,
	//	1.f, 1.f, -1.f,
	//	0.f, 1.f, -1.f,

	//	0.f, 0.f, 0.f,
	//	1.f, 0.f, -1.f,
	//	1.f, 0.f, 0.f,

	//	0.f, 0.f, 0.f,
	//	0.f, 0.f, -1.f,
	//	1.f, 0.f, -1.f,
	//};

	//uint my_id = 0;
	//glGenBuffers(1, (GLuint*)&(my_id));
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapesize , shape, GL_STATIC_DRAW);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//// … bind and use other buffers
	//glDrawArrays(GL_TRIANGLES, 0, shapesize/3);
	//glDisableClientState(GL_VERTEX_ARRAY);

	int index[36] =
	{
		0,1,2,
		0,2,3,
		4,0,3,
		4,3,7,
		1,5,6,
		1,6,2,
		5,4,7,
		5,7,6,
		3,2,6,
		3,6,7,
		0,5,1,
		0,1,5
	};
	
	static const float verts[24] =
	{
		0.f,0.f,0.f,//0
		1.f,0.f,0.f,//1
		1.f,1.f,0.f,//2
		0.f,1.f,0.f,//3
		0.f,0.f,-1.f,//4
		1.f,0.f,-1.f,//5
		1.f,1.f,-1.f,//6
		0.f,1.f,-1.f//7
	};

	glEnableClientState(GL_VERTEX_ARRAY);

	uint my_id = 0;
	glGenBuffers(1, (GLuint*)&(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts) * 3, verts, GL_STATIC_DRAW);
	
	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, sizeof(index), GL_UNSIGNED_BYTE, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	return UPDATE_CONTINUE;
}

