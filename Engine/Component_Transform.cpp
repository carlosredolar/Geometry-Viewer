#include "Component_Transform.h"
#include "ImGui/imgui.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"

Component_Transform::Component_Transform() : Component(ComponentType::TRANSFORM)
{
	position = float3::zero;
	_rotation = Quat::identity;
	_eulerRotation = float3::zero;
	_scale = float3::one;

	_localTransform = float4x4::FromTRS(position, _rotation, _scale);
	_globalTransform = _localTransform;
	_parentGlobalTransform = float4x4::identity;
}

Component_Transform::Component_Transform(float3 position, Quat rotation, float3 scale) : Component(ComponentType::TRANSFORM)
{
	position = position;
	_rotation = rotation;
	_eulerRotation = rotation.ToEulerXYZ();
	_eulerRotation *= RADTODEG;
	_scale = scale;

	_localTransform = float4x4::FromTRS(position, _rotation, _scale);
	_globalTransform = _localTransform;
	_parentGlobalTransform = float4x4::identity;
}

Component_Transform::~Component_Transform() {}

void Component_Transform::Update() {}

void Component_Transform::OnGUI()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float pos[3] = { position.x, position.y, position.z };
		if (ImGui::DragFloat3("Position", pos, 0.1f, -10000.0f, 10000.0f))
		{
			SetPosition(pos[0], pos[1], pos[2]);
			UpdateGlobalTransform();
			ownerGameObject->UpdateChildrenTransforms();
		}

		float rot[3] = { _eulerRotation.x, _eulerRotation.y, _eulerRotation.z };
		if (ImGui::DragFloat3("Rotation", rot, 0.1f, -360.0f, 360.0f))
		{
			SetRotation(rot[0], rot[1], rot[2]);
			UpdateGlobalTransform();
			ownerGameObject->UpdateChildrenTransforms();
		}

		float scl[3] = { _scale.x, _scale.y, _scale.z };
		if (ImGui::DragFloat3("Scale", scl, 0.01f, -10000.0f, 10000.0f))
		{
			SetScale(scl[0], scl[1], scl[2]);
			UpdateGlobalTransform();
			ownerGameObject->UpdateChildrenTransforms();
		};

		ImGui::Spacing();

		if (ImGui::Button("Reset"))
			ResetTransform();

		ImGui::Spacing();
	}
}

void Component_Transform::Save(JsonArray& save_array)
{
	JsonObj save_object;
	save_object.AddInt("Type", type);
	save_object.AddFloat3("Position", position);
	save_object.AddQuaternion("Rotation", _rotation);
	save_object.AddFloat3("Scale", _scale);
	save_array.AddObject(save_object);
}

void Component_Transform::Load(JsonObj& load_object)
{
	position = load_object.GetFloat3("Position");
	_rotation = load_object.GetQuaternion("Rotation");
	_scale = load_object.GetFloat3("Scale");

	UpdateEulerRotation();
	UpdateGlobalTransform();
}

void Component_Transform::Set(float4x4 g_transform)
{
	_localTransform = g_transform;
}

float4x4 Component_Transform::GetLocalTransform()
{
	return _localTransform;
}

float4x4 Component_Transform::GetGlobalTransform()
{
	return _globalTransform;
}

void Component_Transform::SetGlobalTransform(float4x4 newTransform)
{
	_globalTransform = newTransform;
	float4x4 inverseParentGlobal = _parentGlobalTransform;
	inverseParentGlobal.Inverse();
	_localTransform = inverseParentGlobal * _globalTransform;
	UpdateTRS();
	ownerGameObject->UpdateChildrenTransforms();
}

void Component_Transform::UpdateLocalTransform()
{
	_localTransform = float4x4::FromTRS(position, _rotation, _scale);
	UpdateEulerRotation();
}

void Component_Transform::UpdateTRS()
{
	_localTransform.Decompose(position, _rotation, _scale);
	UpdateEulerRotation();
}

void Component_Transform::UpdateGlobalTransform()
{
	UpdateLocalTransform();
	_globalTransform = _parentGlobalTransform * _localTransform;
}

void Component_Transform::UpdateGlobalTransform(float4x4 parentGlobalTransform)
{
	UpdateLocalTransform();
	_parentGlobalTransform = parentGlobalTransform;
	_globalTransform = _parentGlobalTransform * _localTransform;
}

void Component_Transform::ChangeParentTransform(float4x4 newParentGlobalTransform)
{
	_parentGlobalTransform = newParentGlobalTransform;
	newParentGlobalTransform.Inverse();
	_localTransform =  newParentGlobalTransform * _globalTransform;
	UpdateTRS();
	_globalTransform = _parentGlobalTransform * _localTransform;
}

void Component_Transform::ResetTransform()
{
	position = float3::zero;
	_rotation = Quat::identity;
	_scale = float3::one;
	_eulerRotation = float3::zero;

	UpdateGlobalTransform();
	ownerGameObject->UpdateChildrenTransforms();
	
}

void Component_Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	UpdateLocalTransform();
}

void Component_Transform::SetPosition(float3 new_position)
{
	position = new_position;
	UpdateGlobalTransform();
}

float3 Component_Transform::GetPosition()
{
	return position;
}


//Set rotation from Euler angles
void Component_Transform::SetRotation(float x, float y, float z)
{
	_eulerRotation = float3(x, y, z);
	_rotation = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
	UpdateGlobalTransform();
}

void Component_Transform::SetRotation(Quat new_rotation)
{
	_rotation = new_rotation;
	_eulerRotation = _rotation.ToEulerXYZ() * RADTODEG;

	UpdateGlobalTransform();
}

void Component_Transform::SetRotation(float x, float y, float z, float w)
{
	_rotation.x = x;
	_rotation.y = y;
	_rotation.z = z;
	_rotation.w = w;

	UpdateGlobalTransform();
}

Quat Component_Transform::GetRotation()
{
	return _rotation;
}

void Component_Transform::UpdateEulerRotation()
{
	_eulerRotation = _rotation.ToEulerXYZ();
	_eulerRotation *= RADTODEG;
}

void Component_Transform::SetScale(float x, float y, float z)
{
	_scale.x = x;
	_scale.y = y;
	_scale.z = z;

	UpdateGlobalTransform();
}

void Component_Transform::SetScale(float3 new_scale)
{
	_scale = new_scale;

	UpdateGlobalTransform();
}

void Component_Transform::SetProportionalScale(float multiplier)
{
	_scale.x = _scale.y = _scale.z = multiplier;

	UpdateGlobalTransform();
}

float3 Component_Transform::GetScale()
{
	return _scale;
}

