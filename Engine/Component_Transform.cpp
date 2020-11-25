#include "Component_Transform.h"

Component_Transform::Component_Transform(GameObject* ownerGameObject, bool enabled) : Component(ComponentType::TRANSFORM, ownerGameObject, enabled)
{
	position = { 0,0,0 };
	rotation = Quat::identity;
	scale = { 1,1,1 };
	UpdateLocalTransform();
	globalTransform = localTransform;
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

void Component_Transform::SetTransform(float3 pos, Quat rot, float3 scl)
{
	this->position = pos;
	this->rotation = rot;
	this->scale = scl;
	UpdateLocalTransform();
	GenerateEulerFromRot();
}

void Component_Transform::SetTransform(float3 pos, float3 rot, float3 scl)
{
	this->position = pos;
	this->rotation = Quat::FromEulerXYZ(DegToRad(rot.x), DegToRad(rot.y), DegToRad(rot.z));
	this->scale = scl;
	UpdateLocalTransform();
	GenerateEulerFromRot();
}

void Component_Transform::Update()
{

}

void Component_Transform::CleanUp() 
{

}

float4x4 Component_Transform::GetLocalTransform()
{
	return localTransform;
}

float4x4 Component_Transform::GetGlobalTransform()
{
	return globalTransform;
}

void Component_Transform::UpdateLocalTransform()
{
	localTransform = float4x4::FromTRS(position, rotation, scale);
	//globalTransform = localTransform;
}

void Component_Transform::UpdateGlobalTransform(float4x4 parentGlobalTransform)
{
	localTransform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = parentGlobalTransform * localTransform;
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