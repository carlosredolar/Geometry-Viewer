#include "Component_Transform.h"

Component_Transform::Component_Transform(GameObject* ownerGameObject, bool enabled) : Component(COMPONENT_TYPE::TRANSFORM, ownerGameObject, enabled)
{

}

Component_Transform::~Component_Transform()
{
	CleanUp();
}

void Component_Transform::SetTransform(float4x4 transform)
{
	transform.Decompose(position, rotation, scale);
	GenerateEulerFromRot();
}

void Component_Transform::SetTransform(float3 pos, Quat rotation, float3 scale)
{
	this->position = pos;
	this->rotation = rotation;
	this->scale = scale;
	transform = float4x4::FromTRS(position, this->rotation, this->scale);
	GenerateEulerFromRot();
}

void Component_Transform::SetTransform(float3 pos, float3 rotation, float3 scale)
{
	this->position = pos;
	this->rotation.FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	this->scale = scale;
	transform = float4x4::FromTRS(position, this->rotation, this->scale);
	GenerateEulerFromRot();
}

void Component_Transform::Update()
{

}

void Component_Transform::CleanUp() {

}

float4x4 Component_Transform::GetTransform() const
{
	return transform;
}

float3 Component_Transform::GetPosition() const
{
	return position;
}

Quat Component_Transform::GetRotation() const
{
	return rotation;
}

float3 Component_Transform::GetRotationEuler() const
{
	return rotationEuler;
}

float3 Component_Transform::GetScale() const
{
	return scale;
}

void Component_Transform::GenerateEulerFromRot()
{
	rotationEuler = rotation.ToEulerXYZ();
	rotationEuler *= RADTODEG;
}