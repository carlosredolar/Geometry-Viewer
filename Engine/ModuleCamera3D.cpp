#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleJson.h"
#include "Component_Camera.h"

#include "Component_Mesh.h"
#include "GameObject.h"
#include "Component_Transform.h"

#include "Libs/ImGuizmo/ImGuizmo.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "camera";

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	position = float3(40.0f, 15.0f, 45.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	camera = new Component_Camera(nullptr);
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
	movementSpeed = config.GetFloat("move_speed");
	dragSpeed = config.GetFloat("drag_speed");
	orbitSpeed = config.GetFloat("orbit_speed");
	panSpeed = config.GetFloat("pan_speed");
	zoomSpeed = config.GetFloat("zoom_speed");
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
	//if (!App->gui->IsSceneFocused())
	//	return UPDATE_CONTINUE;

	float3 newPos = float3::zero;
	int speed = 1;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 2;

	float scrollSpeed = speed * 4;

	//if (App->input->scrollUp) Position -= Z * scrollSpeed;
	//if (App->input->scrollDown) Position += Z * scrollSpeed;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		GameObject* selectedGameObject = App->scene->GetSelectedGameObject();
		if (selectedGameObject != nullptr)
		{
			float3 center(selectedGameObject->GetComponent<Component_Transform>()->GetPosition().x,
				selectedGameObject->GetComponent<Component_Transform>()->GetPosition().y,
				selectedGameObject->GetComponent<Component_Transform>()->GetPosition().z);

			LookAt(center);
		}
	}

	//Forwards/Backwards
	if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT))
		newPos += camera->GetFrustum().front * movementSpeed * speed * dt;
	if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT))
		newPos -= camera->GetFrustum().front * movementSpeed * speed * dt;

	//Left/Right
	if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT))
		newPos += camera->GetFrustum().WorldRight() * movementSpeed * speed * dt;
	if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT))
		newPos -= camera->GetFrustum().WorldRight() * movementSpeed * speed * dt;

	//Drag
	if ((App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT))
	{
		newPos -= camera->GetFrustum().WorldRight() * App->input->GetMouseXMotion() * dragSpeed * dt;
		newPos += camera->GetFrustum().up * App->input->GetMouseYMotion() * dragSpeed * dt;
	}

	// Zoom 
	if (App->input->GetMouseZ() > 0)
		newPos += camera->GetFrustum().front * zoomSpeed * dt;
	else if (App->input->GetMouseZ() < 0)
		newPos -= camera->GetFrustum().front * zoomSpeed * dt;

	if ((App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT))
		Orbit(dt);
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		Pan(dt);

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsOver()/* && App->gui->MouseOnScene()*/)
	{
		GameObject* pickedGameObject = PickGameObject();
		App->scene->SelectGameObject(pickedGameObject);
	}

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

	Quat y_rotation(camera->GetFrustum().up, dx * dt * orbitSpeed * 0.1f);
	Quat x_rotation(camera->GetFrustum().WorldRight(), dy * dt * orbitSpeed * 0.1f);

	float3 distance = position - reference;
	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);

	position = distance + reference;
	camera->SetPosition(position);
	camera->Look(reference);
}

void ModuleCamera3D::Pan(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Quat y_rotation(camera->GetFrustum().up, dx * dt * panSpeed * 0.1f);
	Quat x_rotation(camera->GetFrustum().WorldRight(), dy * dt * panSpeed * 0.1f);

	float3 distance = position - reference;
	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);

	reference = position - distance;
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

void ModuleCamera3D::SetPosition(float3 newPosition)
{
	position = newPosition;
}

GameObject* ModuleCamera3D::PickGameObject()
{
	/*
	float normalized_x = App->gui->mouseScenePosition.x / App->gui->image_size.x;
	float normalized_y = App->gui->mouseScenePosition.y / App->gui->image_size.y;

	normalized_x = (normalized_x - 0.5f) * 2.0f;
	normalized_y = -(normalized_y - 0.5f) * 2.0f;

	LineSegment my_ray = camera->GetFrustum().UnProjectLineSegment(normalized_x, normalized_y);

	App->renderer3D->_ray = my_ray;

	std::vector<GameObject*> sceneGameObjects = App->scene->GetAllGameObjects();
	std::map<float, GameObject*> hitGameObjects;

	//find all hit GameObjects
	for (size_t i = 0; i < sceneGameObjects.size(); i++)
	{
		bool hit = my_ray.Intersects(sceneGameObjects[i]->GetAABB());

		if (hit)
		{
			float dNear;
			float dFar;
			hit = my_ray.Intersects(sceneGameObjects[i]->GetAABB(), dNear, dFar);
			hitGameObjects[dNear] = sceneGameObjects[i];
		}
	}

	std::map<float, GameObject*>::iterator it = hitGameObjects.begin();
	for (it; it != hitGameObjects.end(); it++)
	{
		GameObject* gameObject = it->second;

		LineSegment ray_local_space = my_ray;
		ray_local_space.Transform(gameObject->GetComponent<Component_Transform>()->GetGlobalTransform().Inverted());

		Component_Mesh* mesh = (Component_Mesh*)gameObject->GetComponent<Component_Mesh>();
		ResourceMesh* resourceMesh = dynamic_cast<ResourceMesh*>(mesh->GetResource(ResourceType::RESOURCE_MESH));

		for (size_t i = 0; i < resourceMesh->amountIndices; i += 3)
		{
			//create every triangle
			float3 v1(resourceMesh->vertices[resourceMesh->indices[i] * 3], resourceMesh->vertices[resourceMesh->indices[i] * 3 + 1],
				resourceMesh->vertices[resourceMesh->indices[i] * 3 + 2]);

			float3 v2(resourceMesh->vertices[resourceMesh->indices[i + 1] * 3], resourceMesh->vertices[resourceMesh->indices[i + 1] * 3 + 1],
				resourceMesh->vertices[resourceMesh->indices[i + 1] * 3 + 2]);

			float3 v3(resourceMesh->vertices[resourceMesh->indices[i + 2] * 3], resourceMesh->vertices[resourceMesh->indices[i + 2] * 3 + 1],
				resourceMesh->vertices[resourceMesh->indices[i + 2] * 3 + 2]);

			const Triangle triangle(v1, v2, v3);

			float distance;
			float3 intersectionPoint;
			if (ray_local_space.Intersects(triangle, &distance, &intersectionPoint)) {
				return gameObject;
			}
		}
	}
	*/
	return nullptr;
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
