#pragma once
#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;

enum FOV {
	VERTICAL_FOV,
	HORIZONTAL_FOV
};

class Component_Camera : public Component {
public:
	Component_Camera();
	Component_Camera(GameObject* gameObject);
	~Component_Camera();

	void Update() override;

	void OnGUI() override;

	void Look(float3 point);
	Frustum GetFrustum();

	float* GetViewMatrix();
	float* GetProjectionMatrix();

	

	//Save/Load JSON
	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	//FOV tools
	void SetFOV(FOV fov);
	void SetVerticalFOV(float verticalFOV, float screenWidth, float screenHeight);
	void SetHorizontalFOV(float horizontalFOV, float screenWidth = 16.0f, float screenHeight = 9.0f);
	void SetCameraPosition(float3 position);
	void SetCameraReference(float3 reference);
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void AdjustFOV();
	void AdjustFOV(float width, float height);


	bool CheckBBOnCamera(AABB& aabb);

public:
	FOV fov;

private:
	Frustum frustum;
	float3 cameraReference;
	float aspectRatio;
	
};
