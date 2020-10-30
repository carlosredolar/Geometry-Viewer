#pragma once
#ifndef __Component_Transform_H__

#include "Component.h"
#include "GameObject.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Component_Transform : public Component {
public:

	//Constructor
	Component_Transform(GameObject* ownerGameObject, bool enabled = true);

	//Destructor
	~Component_Transform();

	//Update
	void Update() override;

	void SetTransform(float4x4 transform);
	void SetTransform(float3 pos, Quat rotation, float3 scale);
	void SetTransform(float3 pos, float3 rotation, float3 scale);

	void CleanUp();

	float4x4	GetTransform() const;
	float3		GetPosition() const;
	Quat		GetRotation() const;
	float3		GetRotationEuler() const;
	float3		GetScale() const;

private:

	void GenerateEulerFromRot();

	//Transformation values
	float4x4 transform;
	float3 position;
	Quat rotation;
	float3 scale;
	float3 rotationEuler;
};

#endif // !__Component_Transform_H__
