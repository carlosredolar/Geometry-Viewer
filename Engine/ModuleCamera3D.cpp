#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleJson.h"
#include "Component_Camera.h"

#include "Component_Mesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "glew/include/glew.h"


ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "camera";

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	position = float3(40.0f, 15.0f, 45.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	camera = new Component_Camera();
	camera->SetPosition(float3(position));
	camera->SetReference(reference);
	Look(reference);

	background = { 0.12f, 0.12f, 0.12f, 1.0f };
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init() {
	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::LoadConfig(JsonObj& config)
{
	move_speed = config.GetFloat("move_speed");
	drag_speed = config.GetFloat("drag_speed");
	orbit_speed = config.GetFloat("orbit_speed");
	zoom_speed = config.GetFloat("zoom_speed");
	sensitivity = config.GetFloat("sensitivity");

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete camera;
	camera = nullptr;

	return true;
}

void ModuleCamera3D::OnResize(int width, int height)
{
	camera->AdjustFieldOfView(width, height);
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (!App->gui->IsSceneFocused())
		return UPDATE_CONTINUE;

	float3 newPos = float3::zero;
	int speed_multiplier = 1;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed_multiplier = 2;

	//Up/Down
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_REPEAT) 
		newPos.y += move_speed * dt;
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT) 
		newPos.y -= move_speed * dt;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) 
	{
		if (App->scene->selectedGameObject != nullptr)
		{
			reference = App->scene->selectedGameObject->GetTransform()->GetPosition();
			LookAt(reference);
		}
	}

	//Forwards/Backwards
	if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)) 
		newPos += camera->GetFrustum().front * move_speed * speed_multiplier * dt;
	if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)) 
		newPos -= camera->GetFrustum().front * move_speed * speed_multiplier * dt;

	//Left/Right
	if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)) 
		newPos += camera->GetFrustum().WorldRight() * move_speed * speed_multiplier * dt;
	if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT))
		newPos -= camera->GetFrustum().WorldRight() * move_speed * speed_multiplier * dt;

	//Drag
	if ((App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT))
	{
		newPos -= camera->GetFrustum().WorldRight() * App->input->GetMouseXMotion() * drag_speed * dt;
		newPos += camera->GetFrustum().up * App->input->GetMouseYMotion() * drag_speed * dt;
	}

	// Zoom 
	float distanceToReference = Distance(position, reference);
	if (distanceToReference > 0.05f) 
	{
		if (App->input->GetMouseZ() > 0)
		{
			position += camera->GetFrustum().front * zoom_speed * distanceToReference * dt;
			LookAt(reference);
		}
		else if (App->input->GetMouseZ() < 0)
		{
			position -= camera->GetFrustum().front * zoom_speed * distanceToReference * dt;
			LookAt(reference);
		}
	}
	else if (App->input->GetMouseZ() < 0)
	{
		position -= camera->GetFrustum().front * zoom_speed * distanceToReference * dt;
		LookAt(reference);
	}


	if ((App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT))
	{
		Orbit(dt);
	} 
		
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		SelectGO();
	}
		
	RenderRay();

	position += newPos;
	camera->SetPosition(position);
	reference += newPos;
	camera->SetReference(reference);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(float3& position)
{
	camera->Look(position);
	reference = position;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& Spot)
{
	camera->Look(Spot);
	reference = Spot;
}

Component_Camera* ModuleCamera3D::GetCamera() { return camera; }

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	position += Movement;
	reference += Movement;
}

void ModuleCamera3D::Orbit(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Quat y_rotation(camera->GetFrustum().up, dx * dt * orbit_speed * 0.1f);
	Quat x_rotation(camera->GetFrustum().WorldRight(), dy * dt * orbit_speed * 0.1f);

	float3 distance = position - reference;
	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);

	position = distance + reference;
	camera->SetPosition(position);
	camera->Look(reference);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return camera->GetViewMatrix();
}

float4x4 ModuleCamera3D::GetViewMatrixM()
{
	return camera->GetFrustum().ViewMatrix();
}

float* ModuleCamera3D::GetProjectionMatrix()
{
	return camera->GetProjectionMatrix();
}

float4x4 ModuleCamera3D::GetProjectionMatrixM()
{
	return camera->GetFrustum().ProjectionMatrix();
}

float3 ModuleCamera3D::GetPosition()
{
	return position;
}

FixedFOV ModuleCamera3D::GetFixedFOV()
{
	return camera->fixedFOV;
}

void ModuleCamera3D::SetFixedFOV(FixedFOV fixedFOV)
{
	camera->SetFixedFOV(fixedFOV);
}

float ModuleCamera3D::GetVerticalFieldOfView()
{
	return camera->GetFrustum().verticalFov;
}

float ModuleCamera3D::GetHorizontalFieldOfView()
{
	return camera->GetFrustum().horizontalFov;
}


void ModuleCamera3D::SetVerticalFieldOfView(float verticalFOV, int screen_width, int screen_height)
{
	camera->SetVerticalFieldOfView(verticalFOV, screen_width, screen_height);
	App->renderer3D->UpdateProjectionMatrix(camera->GetProjectionMatrix());
}

void ModuleCamera3D::SetHorizontalFieldOfView(float horizontalFOV, int screen_width, int screen_height)
{
	camera->SetHorizontalFieldOfView(horizontalFOV, screen_width, screen_height);
	App->renderer3D->UpdateProjectionMatrix(camera->GetProjectionMatrix());
}

void ModuleCamera3D::Reset()
{
	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 0.0f, 5.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	Move(float3(1.0f, 1.0f, 0.0f));
	LookAt(float3(0.0f, 0.0f, 0.0f));
}

void ModuleCamera3D::SetBackgroundColor(float r, float g, float b, float w)
{
	background = { r,g,b,w };
}

GameObject* ModuleCamera3D::SelectGO() 
{

	float normalized_x = App->gui->mouseScenePosition.x / App->gui->image_size.x;
	float normalized_y = App->gui->mouseScenePosition.y / App->gui->image_size.y;

	LineSegment picking = App->camera->camera->GetFrustum().UnProjectLineSegment(normalized_x, normalized_y);

	ray = LineSegment();

	LOG("X: %.1f Y: %.1f", normalized_x, normalized_y);

	return nullptr;
}

void ModuleCamera3D::RenderRay() 
{
	glBegin(GL_LINES);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ray.a.x, ray.a.y, ray.a.z);
	glVertex3f(ray.b.x, ray.b.y, ray.b.z);
	glEnd();
}

