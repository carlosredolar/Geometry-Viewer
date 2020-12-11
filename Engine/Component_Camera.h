#pragma once
#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;

enum FixedFOV {
	FIXED_VERTICAL_FOV,
	FIXED_HORIZONTAL_FOV
};

class Component_Camera : public Component {
public:
	Component_Camera();
	Component_Camera(GameObject* gameObject);
	~Component_Camera();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& save_array) override;
	void Load(JsonObj& load_object) override;

	void SetFixedFOV(FixedFOV fixedFOV);
	void AdjustFieldOfView();
	void AdjustFieldOfView(float width, float height);
	void SetVerticalFieldOfView(float verticalFOV, float screen_width, float screen_height);
	void SetHorizontalFieldOfView(float horizontalFOV, float screen_width = 16.0f, float screen_height = 9.0f);
	void SetPosition(float3 position);
	void SetReference(float3 reference);
	void SetNearPlaneDistance(float distance);
	void SetFarPlaneDistance(float distance);

	void Look(float3 spot);
	Frustum GetFrustum();

	float* GetViewMatrix();
	float* GetProjectionMatrix();
	bool ContainsAABB(AABB& aabb);
	//virtual void Enable() override;
	//virtual void Disable() override;

public:
	FixedFOV fixedFOV;

private:
	Frustum frustum;
	float _aspectRatio;
	float3 _reference;
};
