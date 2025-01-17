#pragma once
#include "Module.h"
#include "Globals.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Camera;
class GameObject;
enum FOV;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void ScreenResized(int width, int height);

	bool LoadConfig(JsonObj& config) override;

	void Look(float3& position);
	void LookAt(const float3& Spot);
	Component_Camera* GetCamera();
	float* GetViewMatrix();
	float4x4 GetViewMatrixM();
	float* GetProjectionMatrix();
	float4x4 GetProjectionMatrixM();
	float3 GetPosition();
	float3 GetReferencePos();
	GameObject* SelectGO();

	FOV GetFixedFOV();
	void SetPosition(float3 pos);
	void SetReferencePos(float3 ref);
	void SetFixedFOV(FOV fixedFOV);
	float GetVerticalFieldOfView();
	float GetHorizontalFieldOfView();
	void SetVerticalFieldOfView(float verticalFOV, int screen_width, int screen_height);
	void SetHorizontalFieldOfView(float horizontalFOV, int screen_width, int screen_height);

	void RenderRay();

	void Reset();
	void SetBackgroundColor(float r, float g, float b, float w);

private:
	void Move(const float3& Movement);
	void Orbit(float dt);

public:
	Color background;

	float movementSpeed;
	float panningSpeed;
	float orbitingSpeed;
	float zoomSpeed;
	float sensitivity;

	LineSegment ray;

private:
	//mat4x4 ViewMatrix, ViewMatrixInverse;
	float3 X, Y, Z;
	Component_Camera* camera;
	float3 position;
	float3 reference;
};