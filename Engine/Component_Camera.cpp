#include "Component_Camera.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include "ModuleJson.h"

Component_Camera::Component_Camera() : Component(ComponentType::CAMERA, nullptr), _aspectRatio(16.0f/9.0f), fixedFOV(FIXED_HORIZONTAL_FOV) 
{
	frustum.type = FrustumType::PerspectiveFrustum;
	
	frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.up = float3(0.0f, 1.0f, 0.0f);
	frustum.front = float3(0.0f, 0.0f, 1.0f);

	frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	frustum.nearPlaneDistance = 0.3f;
	frustum.farPlaneDistance = 1000.0f;
}

Component_Camera::Component_Camera(GameObject* gameObject) : Component(ComponentType::CAMERA, gameObject), _aspectRatio(16.0f/9.0f), fixedFOV(FIXED_HORIZONTAL_FOV)
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.up = float3(0.0f, 1.0f, 0.0f);
	frustum.front = float3(0.0f, 0.0f, 1.0f);

	frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	frustum.nearPlaneDistance = 0.3f;
	frustum.farPlaneDistance = 1000.0f;
}

Component_Camera::~Component_Camera(){}

void Component_Camera::Update()
{
	frustum.pos = ownerGameObject->GetTransform()->GetPosition();
	
	frustum.up = ownerGameObject->GetTransform()->GetGlobalTransform().WorldY();
	frustum.front = ownerGameObject->GetTransform()->GetGlobalTransform().WorldZ();

	float3 corner_points[8];
	frustum.GetCornerPoints(corner_points);
	
	App->renderer3D->DrawAABB(corner_points);

	
}

void Component_Camera::OnGUI()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		int fov = fixedFOV;

		const char* items[] = { "Vertical", "Horizontal" };
		if (ImGui::Combo("Fixed FOV", &fov, items, IM_ARRAYSIZE(items)))
		{
			std::string currentFov = std::string(items[fov]);
			if (currentFov == "Vertical") SetFixedFOV(FixedFOV::FIXED_VERTICAL_FOV);
			else if (currentFov == "Horizontal") SetFixedFOV(FixedFOV::FIXED_HORIZONTAL_FOV);
		}

		ImGui::Spacing();

		bool currentFixedFOV = fixedFOV == FixedFOV::FIXED_VERTICAL_FOV;

		//Fixed Vertical FOV Settings
		if (currentFixedFOV)
		{
			float verticalFOV = frustum.verticalFov * RADTODEG;
			if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
			{
				frustum.verticalFov = verticalFOV * DEGTORAD;
				frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (_aspectRatio));
			}

			ImGui::Spacing();
			ImGui::Text("Horizontal FOV: %.2f", frustum.horizontalFov * RADTODEG);
		}
		//Fixed Horizontal FOV Settings
		else
		{
			float horizontalFOV = frustum.horizontalFov * RADTODEG;
			if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
			{
				frustum.horizontalFov = horizontalFOV * DEGTORAD;
				frustum.verticalFov = 2.0f * std::atan(std::tan(frustum.horizontalFov * 0.5f) * (1 / _aspectRatio));
			}
			ImGui::Spacing();
			ImGui::Text("Vertical FOV: %.2f", frustum.verticalFov * RADTODEG);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::DragFloat("Near Plane", &frustum.nearPlaneDistance, 0.05f, 100.0f);
		ImGui::DragFloat("Far Plane", &frustum.farPlaneDistance, 5.0f, 2000.0f);

		ImGui::Spacing();
	}
}

void Component_Camera::Save(JsonArray& save_array)
{
	JsonObj save_object;

	save_object.AddString("name", name.c_str());
	save_object.AddInt("Type", type);
	bool mainCamera = App->renderer3D->GetMainCamera() == this;
	save_object.AddBool("Main Camera", mainCamera);
	save_object.AddFloat3("position", frustum.pos);
	save_object.AddFloat3("up", frustum.up);
	save_object.AddFloat3("front", frustum.front);
	save_object.AddFloat("horizontalFOV", frustum.horizontalFov * DEGTORAD);
	save_object.AddFloat("verticalFOV", frustum.verticalFov * DEGTORAD);
	save_object.AddFloat("nearPlane", frustum.nearPlaneDistance);
	save_object.AddFloat("farPlane", frustum.farPlaneDistance);
	save_object.AddFloat("aspectRatio", _aspectRatio);

	save_array.AddObject(save_object);
}

void Component_Camera::Load(JsonObj& load_object)
{
	if (load_object.GetBool("Main Camera", false))
		App->renderer3D->SetMainCamera(this);
	frustum.pos = load_object.GetFloat3("position");
	frustum.up = load_object.GetFloat3("up");
	frustum.front = load_object.GetFloat3("front");
	frustum.horizontalFov = load_object.GetFloat("horizontalFOV") * RADTODEG;
	frustum.verticalFov = load_object.GetFloat("verticalFOV") * RADTODEG;
	frustum.nearPlaneDistance = load_object.GetFloat("nearPlane");
	frustum.farPlaneDistance = load_object.GetFloat("farPlane");
}

void Component_Camera::SetFixedFOV(FixedFOV g_fixedFOV)
{
	fixedFOV = g_fixedFOV;
}

void Component_Camera::AdjustFieldOfView()
{
	frustum.verticalFov = 2 * atan(tan(frustum.horizontalFov * 0.5f) * (1/_aspectRatio));
	/*
	switch (_aspectRatio)
	{
	case AR_16_9:
		//frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (16.0f / 9.0f));
		frustum.verticalFov = 2 * atan(tan(frustum.horizontalFov * 0.5f) * (9.0f / 16.0f));
		break;
	case AR_16_10:
		//frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (16.0f / 10.0f));
		frustum.verticalFov = 2 * atan(tan(frustum.horizontalFov * 0.5f) * (10.0f / 16.0f));
		break;
	case AR_4_3:
		//frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (4.0f / 3.0f));
		frustum.verticalFov = 2 * atan(tan(frustum.horizontalFov * 0.5f) * (3.0f / 4.0f));
		break;
	default:
		break;
	}
	*/
}

void Component_Camera::AdjustFieldOfView(float width, float height)
{
	if(fixedFOV == FIXED_HORIZONTAL_FOV)
		frustum.verticalFov = 2.0f * std::atan(std::tan(frustum.horizontalFov * 0.5f) * (height / width));
	else 
		frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (width / height));
}

void Component_Camera::SetVerticalFieldOfView(float verticalFOV, float screen_width, float screen_height)
{
	frustum.verticalFov = verticalFOV;
	frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (screen_width / screen_height));
}

void Component_Camera::SetHorizontalFieldOfView(float horizontalFOV, float screen_width, float screen_height)
{
	frustum.horizontalFov = horizontalFOV;
	frustum.verticalFov = 2.0f * std::atan(std::tan(frustum.horizontalFov * 0.5f) * (screen_height / screen_width));
}

void Component_Camera::SetPosition(float3 position)
{
	frustum.pos = position;
}

void Component_Camera::SetReference(float3 reference)
{
	_reference = reference;
}

void Component_Camera::SetNearPlaneDistance(float distance)
{
	frustum.nearPlaneDistance = distance;
}

void Component_Camera::SetFarPlaneDistance(float distance)
{
	frustum.farPlaneDistance = distance;
}

void Component_Camera::Look(float3 spot)
{
	float3 difference = spot - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, difference.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}

Frustum Component_Camera::GetFrustum()
{
	return frustum;
}

float* Component_Camera::GetViewMatrix()
{
	float4x4 viewMatrix;

	viewMatrix = frustum.ViewMatrix();
	viewMatrix.Transpose();

	return (float*)viewMatrix.v;
}

float* Component_Camera::GetProjectionMatrix()
{
	float4x4 projectionMatrix;

	projectionMatrix = frustum.ProjectionMatrix();
	projectionMatrix.Transpose();

	return (float*)projectionMatrix.v;
}

bool Component_Camera::ContainsAABB(AABB& aabb)
{
	float3 cornerPoints[8];
	int totalInside = 0;

	//get frustum planes
	Plane* m_plane = new Plane[6](); 
	frustum.GetPlanes(m_plane);

	//get AABB points
	aabb.GetCornerPoints(cornerPoints); 

	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;

		for (int i = 0; i < 8; ++i) 
		{
			if (m_plane[p].normal.Dot(cornerPoints[i]) - m_plane[p].d >= 0.0f) 
			{
				iPtIn = 0;
				--iInCount;
			}
		}
		
		if(iInCount == 0)
			return false;

		totalInside += iPtIn;
	}

	//Totally inside camera view
	if (totalInside == 6)
		return true;

	//Partially inside camera view
	return true;
}
