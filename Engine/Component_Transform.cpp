#include "Component_Transform.h"
#include "ImGui/imgui.h"

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

void Component_Transform::Reset()
{
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;
	rotationEuler = float3::zero;
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

void Component_Transform::OnGUI()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float pos[4] = { position.x, position.y, position.z, 1.0f };
		float rot[4] = { rotationEuler.x, rotationEuler.y, rotationEuler.z, 1.0f };
		float scl[4] = { scale.x, scale.y, scale.z, 1.0f };

		ImGui::DragFloat3("Position", pos, 0.1f, -10000.0f, 10000.0f);
		ImGui::DragFloat3("Rotation", rot, 0.1f, -360.0f, 360.0f);
		ImGui::DragFloat3("Scale", scl, 0.01f, -10000.0f, 10000.0f);

		float3 pos_ = { pos[0], pos[1], pos[2] };
		float3 rot_ = { rot[0], rot[1], rot[2] };
		float3 scl_ = { scl[0], scl[1], scl[2] };

		SetTransform(pos_, rot_, scl_);

		ImGui::Spacing();

		if (ImGui::Button("Reset"))
			Reset();

		ImGui::Spacing();
	}
}