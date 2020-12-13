#include "Component_Camera.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include "ModuleJson.h"

Component_Camera::Component_Camera() : Component(ComponentType::CAMERA, nullptr), aspectRatio(16.0f/9.0f), fov(HORIZONTAL_FOV) 
{
	//Main Camera
	frustum.type = FrustumType::PerspectiveFrustum;
	
	frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.up = float3(0.0f, 1.0f, 0.0f);
	frustum.front = float3(0.0f, 0.0f, 1.0f);


	frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFOV();

	frustum.nearPlaneDistance = 0.2f;
	frustum.farPlaneDistance = 2000.0f;
}

Component_Camera::Component_Camera(GameObject* gameObject) : Component(ComponentType::CAMERA, gameObject), aspectRatio(16.0f/9.0f), fov(HORIZONTAL_FOV)
{
	//Camera on scene
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.up = float3(0.0f, 1.0f, 0.0f);
	frustum.front = float3(0.0f, 0.0f, 1.0f);


	frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFOV();

	frustum.nearPlaneDistance = 0.5f;
	frustum.farPlaneDistance = 500.0f;
}

Component_Camera::~Component_Camera(){}

void Component_Camera::Update()
{
	frustum.pos = ownerGameObject->GetTransform()->GetPosition();
	frustum.up = ownerGameObject->GetTransform()->GetGlobalTransform().WorldY();
	frustum.front = ownerGameObject->GetTransform()->GetGlobalTransform().WorldZ();

	//Draw camera on scene
	float3 corner_points[8];
	frustum.GetCornerPoints(corner_points);
	App->renderer3D->DrawAABB(corner_points, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void Component_Camera::OnGUI()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		int fovValue = fov;

		const char* items[] = { "Vertical", "Horizontal" };
		if (ImGui::Combo("Fixed FOV", &fovValue, items, IM_ARRAYSIZE(items)))
		{
			std::string currentFov = std::string(items[fovValue]);
			if (currentFov == "Vertical") SetFOV(FOV::VERTICAL_FOV);
			else if (currentFov == "Horizontal") SetFOV(FOV::HORIZONTAL_FOV);
		}

		ImGui::Spacing();

		bool currentFOV = fov == FOV::VERTICAL_FOV;

		//HorizontalFOV slider
		if (currentFOV)
		{
			float verticalFOV = frustum.verticalFov * RADTODEG;
			if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
			{
				frustum.verticalFov = verticalFOV * DEGTORAD;
				frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (aspectRatio));
			}
			ImGui::Spacing();

			ImGui::Text("Horizontal FOV: %.2f", frustum.horizontalFov * RADTODEG);
			ImGui::Spacing();
		}

		//VerticalFOV slider
		else
		{
			float horizontalFOV = frustum.horizontalFov * RADTODEG;
			if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
			{
				frustum.horizontalFov = horizontalFOV * DEGTORAD;
				frustum.verticalFov = 2.0f * std::atan(std::tan(frustum.horizontalFov * 0.5f) * (1 / aspectRatio));
			}
			ImGui::Spacing();

			ImGui::Text("Vertical FOV: %.2f", frustum.verticalFov * RADTODEG);
			ImGui::Spacing();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SliderFloat("Near Plane", &frustum.nearPlaneDistance, 0.05f, 100.0f);
		ImGui::SliderFloat("Far Plane", &frustum.farPlaneDistance, 5.0f, 1000.0f);
	}
}

void Component_Camera::Look(float3 point)
{
	float3 difference = point - frustum.pos;

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

void Component_Camera::Save(JsonArray& saveArray)
{
	JsonObj saveObject;

	saveObject.AddString("name", name.c_str());
	saveObject.AddInt("Type", type);
	bool mainCamera = App->renderer3D->GetMainCamera() == this;
	saveObject.AddBool("Main Camera", mainCamera);
	saveObject.AddFloat3("position", frustum.pos);
	saveObject.AddFloat3("up", frustum.up);
	saveObject.AddFloat3("front", frustum.front);
	saveObject.AddFloat("horizontalFOV", frustum.horizontalFov * DEGTORAD);
	saveObject.AddFloat("verticalFOV", frustum.verticalFov * DEGTORAD);
	saveObject.AddFloat("nearPlane", frustum.nearPlaneDistance);
	saveObject.AddFloat("farPlane", frustum.farPlaneDistance);
	saveObject.AddFloat("aspectRatio", aspectRatio);

	saveArray.AddObject(saveObject);
}

void Component_Camera::Load(JsonObj& loadObject)
{
	if (loadObject.GetBool("Main Camera", false))
		App->renderer3D->SetMainCamera(this);
	frustum.pos = loadObject.GetFloat3("position");
	frustum.up = loadObject.GetFloat3("up");
	frustum.front = loadObject.GetFloat3("front");
	frustum.horizontalFov = loadObject.GetFloat("horizontalFOV") * RADTODEG;
	frustum.verticalFov = loadObject.GetFloat("verticalFOV") * RADTODEG;
	frustum.nearPlaneDistance = loadObject.GetFloat("nearPlane");
	frustum.farPlaneDistance = loadObject.GetFloat("farPlane");
}

void Component_Camera::SetFOV(FOV newFOV)
{
	fov = newFOV;
}

void Component_Camera::SetVerticalFOV(float verticalFOV, float screenWidth, float screenHeight)
{
	frustum.verticalFov = verticalFOV;
	frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (screenWidth / screenHeight));
}

void Component_Camera::SetHorizontalFOV(float horizontalFOV, float screenWidth, float screenHeight)
{
	frustum.horizontalFov = horizontalFOV;
	frustum.verticalFov = 2.0f * std::atan(std::tan(frustum.horizontalFov * 0.5f) * (screenHeight / screenWidth));
}

void Component_Camera::SetCameraPosition(float3 position)
{
	frustum.pos = position;
}

void Component_Camera::SetCameraReference(float3 reference)
{
	cameraReference = reference;
}

void Component_Camera::SetNearPlane(float distance)
{
	frustum.nearPlaneDistance = distance;
}

void Component_Camera::SetFarPlane(float distance)
{
	frustum.farPlaneDistance = distance;
}

void Component_Camera::AdjustFOV()
{
	frustum.verticalFov = 2 * atan(tan(frustum.horizontalFov * 0.5f) * (1 / aspectRatio));
}

void Component_Camera::AdjustFOV(float width, float height)
{
	if (fov == HORIZONTAL_FOV)
		frustum.verticalFov = 2.0f * std::atan(std::tan(frustum.horizontalFov * 0.5f) * (height / width));
	else
		frustum.horizontalFov = 2.0f * std::atan(std::tan(frustum.verticalFov * 0.5f) * (width / height));
}

bool Component_Camera::CheckBBOnCamera(AABB& aabb)
{
	float3 cornerPoints[8];
	int totalPointsIn = 0;

	Plane* plane = new Plane[6](); 
	frustum.GetPlanes(plane);

	aabb.GetCornerPoints(cornerPoints); 

	for (int i = 0; i < 6; ++i) {
		int pointCount = 8;
		int pointIn = 1;

		for (int j = 0; j < 8; ++j) 
		{
			if (plane[i].normal.Dot(cornerPoints[j]) - plane[i].d >= 0.0f) 
			{
				pointIn = 0;
				--pointCount;
			}
		}
		
		if(pointCount == 0)
			return false;

		totalPointsIn += pointIn;
	}

	return true;
}
