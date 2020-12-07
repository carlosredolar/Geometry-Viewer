#include "Component_Camera.h"
#include "Component_Transform.h"
#include "Application.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "ModuleRenderer3D.h"
#include "ModuleJson.h"

Component_Camera::Component_Camera() : Component(), aspectRatio(AspectRatio::AR_16_9), fixedFOV(FIXED_HORIZONTAL_FOV) {
	type = ComponentType::CAMERA;

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.up = float3(0.0f, 1.0f, 0.0f);
	frustum.front = float3(0.0f, 0.0f, 1.0f);

	frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	frustum.nearPlaneDistance = 0.3f;
	frustum.farPlaneDistance = 1000.0f;
}

Component_Camera::Component_Camera(GameObject * gameObject) : Component(ComponentType::CAMERA, gameObject), aspectRatio(AspectRatio::AR_16_9)
{
	type = ComponentType::CAMERA;

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.up = float3(0.0f, 1.0f, 0.0f);
	frustum.front = float3(0.0f, 0.0f, 1.0f);

	frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	frustum.nearPlaneDistance = 0.3f;
	frustum.farPlaneDistance = 1000.0f;
}

Component_Camera::~Component_Camera() {}

void Component_Camera::Update()
{
	frustum.pos = ownerGameObject->GetComponent<Component_Transform>()->GetPosition();

	frustum.up = ownerGameObject->GetComponent<Component_Transform>()->GetGlobalTransform().WorldY();
	frustum.front = ownerGameObject->GetComponent<Component_Transform>()->GetGlobalTransform().WorldZ();

	float3 corner_points[8];
	frustum.GetCornerPoints(corner_points);
	App->renderer3D->DrawAABB(corner_points);
}

void Component_Camera::OnGUI()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float horizontalFOV = frustum.horizontalFov * RADTODEG;
		if (ImGui::DragFloat("Horizontal FOV", &horizontalFOV, 0.02f, 0.0f, 130.0f)) {
			frustum.horizontalFov = horizontalFOV * DEGTORAD;
			AdjustFieldOfView();
		}

		float verticalFOV = frustum.verticalFov * RADTODEG;
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV, 0.02f, 0.0f, 60.0f)) {
			frustum.verticalFov = verticalFOV * DEGTORAD;
			AdjustFieldOfView();
		}

		ImGui::Spacing();
	}
}

void Component_Camera::Save(JsonArray & save_array)
{
	JsonObj save_object;

	save_object.AddInt("Type", type);
	bool mainCamera = App->renderer3D->GetMainCamera() == this;
	save_object.AddBool("Main Component_Camera", mainCamera);

	save_array.AddObject(save_object);
}

void Component_Camera::Load(JsonObj & load_object)
{
	if (load_object.GetBool("Main Component_Camera", false))
		App->renderer3D->SetMainCamera(this);
}

void Component_Camera::SetFixedFOV(FixedFOV g_fixedFOV)
{
	fixedFOV = g_fixedFOV;
}

void Component_Camera::AdjustFieldOfView()
{
	switch (aspectRatio)
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
}

void Component_Camera::AdjustFieldOfView(float width, float height)
{
	if (fixedFOV == FIXED_HORIZONTAL_FOV)
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
	reference = reference;
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

bool Component_Camera::ContainsAABB(AABB & aabb)
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

		if (iInCount == 0)
			return false;

		totalInside += iPtIn;
	}

	//Totally inside Component_Camera view
	if (totalInside == 6)
		return true;

	//Partially inside Component_Camera view
	return true;
}