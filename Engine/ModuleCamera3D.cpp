#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleJson.h"
#include "Component_Camera.h"

#include "Component_Mesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Libs/Glew/include/glew.h"


ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "camera";

	position = float3(40.0f, 50.0f, 100.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	camera = new Component_Camera();
	camera->SetCameraPosition(float3(position));
	camera->SetCameraReference(reference);
	Look(reference);

	background = { 0.15f, 0.15f, 0.15f, 1.0f };
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init() {
	return true;
}

bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::LoadConfig(JsonObj& config)
{
	movementSpeed = config.GetFloat("move_speed");
	panningSpeed = config.GetFloat("pan_speed");
	orbitingSpeed = config.GetFloat("orbit_speed");
	zoomSpeed = config.GetFloat("zoom_speed");
	sensitivity = config.GetFloat("sensitivity");

	return true;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning scene camera");
	delete camera;
	camera = nullptr;
	return true;
}

void ModuleCamera3D::ScreenResized(int width, int height)
{
	camera->AdjustFOV(width, height);
}

update_status ModuleCamera3D::Update(float dt)
{
	if (App->in_game)
	{
		camera->SetCameraPosition(App->renderer3D->GetMainCamera()->ownerGameObject->GetTransform()->GetPosition());
		camera->Look(App->renderer3D->GetMainCamera()->GetFrustum().CenterPoint());

		if (App->gui->MouseOnScene() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			if (App->in_game) SelectGO()->Clicked();
		}
	}
	else
	{
		if (!App->gui->IsSceneFocused())
			return UPDATE_CONTINUE;

		float3 newPosition = float3::zero;
		int speedMultiplier = 5;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speedMultiplier *= 2;

		if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT))
			newPosition += camera->GetFrustum().front * movementSpeed * speedMultiplier * dt;
		if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT))
			newPosition -= camera->GetFrustum().front * movementSpeed * speedMultiplier * dt;
		if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT))
			newPosition += camera->GetFrustum().WorldRight() * movementSpeed * speedMultiplier * dt;
		if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT))
			newPosition -= camera->GetFrustum().WorldRight() * movementSpeed * speedMultiplier * dt;
		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT)
			newPosition.y += movementSpeed * speedMultiplier * dt;
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT)
			newPosition.y -= movementSpeed * speedMultiplier * dt;

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			if (App->scene->selectedGameObject != nullptr)
			{
				reference = App->scene->selectedGameObject->GetTransform()->GetPosition();
				LookAt(reference);
			}
		}

		if (App->gui->MouseOnScene()) {
			//Zoom 
			float distanceToReference = Distance(position, reference);
			if (distanceToReference > 0.05f)
			{
				if (App->input->GetMouseZ() > 0)
				{
					position += camera->GetFrustum().front * zoomSpeed * distanceToReference * dt;
					LookAt(reference);
				}
				else if (App->input->GetMouseZ() < 0)
				{
					position -= camera->GetFrustum().front * zoomSpeed * distanceToReference * dt;
					LookAt(reference);
				}
			}
			else if (App->input->GetMouseZ() < 0)
			{
				position -= camera->GetFrustum().front * zoomSpeed * distanceToReference * dt;
				LookAt(reference);
			}

			//Pan
			if ((App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT))
			{
				newPosition -= camera->GetFrustum().WorldRight() * App->input->GetMouseXMotion() * panningSpeed * distanceToReference * dt;
				newPosition += camera->GetFrustum().up * App->input->GetMouseYMotion() * panningSpeed * distanceToReference * dt;
			}

			if ((App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT))
			{
				Orbit(dt);
			}

			if (!(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) && !(ImGuizmo::IsOver()))
			{
				App->scene->selectedGameObject = SelectGO();
			}
		}

		RenderRay();

		position += newPosition;
		camera->SetCameraPosition(position);
		reference += newPosition;
		camera->SetCameraReference(reference);
		camera->Look(reference);
	}


	return UPDATE_CONTINUE;
}


void ModuleCamera3D::Look(float3& position)
{
	camera->Look(position);
	reference = position;
}


void ModuleCamera3D::LookAt(const float3& Spot)
{
	camera->Look(Spot);
	reference = Spot;
}

Component_Camera* ModuleCamera3D::GetCamera() { return camera; }


void ModuleCamera3D::Move(const float3& Movement)
{
	position += Movement;
	reference += Movement;
}

void ModuleCamera3D::Orbit(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();
	Quat x_rotation(camera->GetFrustum().WorldRight(), dy * dt * orbitingSpeed * 0.1f);
	Quat y_rotation(camera->GetFrustum().up, dx * dt * orbitingSpeed * 0.1f);
	float3 distance = position - reference;
	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);
	position = distance + reference;
	camera->SetCameraPosition(position);
	camera->Look(reference);
}

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

float3 ModuleCamera3D::GetReferencePos()
{
	return reference;
}

void ModuleCamera3D::SetPosition(float3 pos)
{
	pos = position;
}

void ModuleCamera3D::SetReferencePos(float3 ref)
{
	ref = reference;
}

FOV ModuleCamera3D::GetFixedFOV()
{
	return camera->fov;
}

void ModuleCamera3D::SetFixedFOV(FOV fixedFOV)
{
	camera->SetFOV(fixedFOV);
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
	camera->SetVerticalFOV(verticalFOV, screen_width, screen_height);
	App->renderer3D->UpdateProjectionMatrix(camera->GetProjectionMatrix());
}

void ModuleCamera3D::SetHorizontalFieldOfView(float horizontalFOV, int screen_width, int screen_height)
{
	camera->SetHorizontalFOV(horizontalFOV, screen_width, screen_height);
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
	float normalized_x = ((App->gui->mouseScenePosition.x / App->gui->sceneRenderSize.x) - 0.5f)*2;
	float normalized_y = -((App->gui->mouseScenePosition.y / App->gui->sceneRenderSize.y) - 0.5f)*2;

	ray = camera->GetFrustum().UnProjectLineSegment(normalized_x, normalized_y);

	//LOG("X: %.1f Y: %.1f", normalized_x, normalized_y); //click point pos

	std::vector<GameObject*> sceneGO = App->scene->GetAllGameObjects();
	float nearestIntersectDist = camera->GetFrustum().farPlaneDistance;
	int nearestGOIndex = -1;
	bool hit = false;
	
	for (int i = 0; i < sceneGO.size(); i++)
	{
		//if (strcmp(sceneGO[i]->GetName(), "Image") == 0)
		//	LOG("Mouse picking ui test");

		hit = ray.Intersects(sceneGO[i]->GetAABB());
		
		if (hit)
		{
			float entranceDist;
			float exitDist;

			hit = ray.Intersects(sceneGO[i]->GetAABB(), entranceDist, exitDist);

			LineSegment localRay = ray;
			localRay.Transform(sceneGO[i]->GetTransform()->GetGlobalTransform().Inverted());
			if (!sceneGO[i]->GetTransform()->IsTransform2D())
			{
				ResourceMesh* mesh = nullptr; mesh = (ResourceMesh*)sceneGO[i]->GetComponent<Component_Mesh>()->GetResource(ResourceType::RESOURCE_MESH);

				if (mesh != nullptr) {
					for (int j = 0; j < mesh->amountIndices; j += 3)
					{
						float3 v1(mesh->vertices[mesh->indices[j] * 3], mesh->vertices[mesh->indices[j] * 3 + 1], mesh->vertices[mesh->indices[j] * 3 + 2]);
						float3 v2(mesh->vertices[mesh->indices[j + 1] * 3], mesh->vertices[mesh->indices[j + 1] * 3 + 1], mesh->vertices[mesh->indices[j + 1] * 3 + 2]);
						float3 v3(mesh->vertices[mesh->indices[j + 2] * 3], mesh->vertices[mesh->indices[j + 2] * 3 + 1], mesh->vertices[mesh->indices[j + 2] * 3 + 2]);
						const Triangle triangle(v1, v2, v3);

						float dist;
						float3 intersectionPoint;
						if (localRay.Intersects(triangle, &dist, &intersectionPoint))
						{
							if (entranceDist < nearestIntersectDist)
							{
								nearestIntersectDist = entranceDist;
								nearestGOIndex = i;
							}
						}
					}
				}
			}
			else
			{
				if (entranceDist < nearestIntersectDist)
				{
					nearestIntersectDist = entranceDist;
					nearestGOIndex = i;
				}
			}
								
		}
	}

	if (nearestGOIndex !=-1) return sceneGO[nearestGOIndex];

	return nullptr;
}

void ModuleCamera3D::RenderRay() 
{
	glColor3f(.0f, 1.0f, .0f);
	glBegin(GL_LINES);
	glVertex3f(ray.a.x, ray.a.y, ray.a.z);
	glVertex3f(ray.b.x, ray.b.y, ray.b.z);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
}

