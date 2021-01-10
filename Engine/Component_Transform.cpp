#include "Component_Transform.h"
#include "Component_Graphic.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "Libs/ImGui/imgui.h"

Component_Transform::Component_Transform(bool isTransform2D) : Component(ComponentType::TRANSFORM)
{
	is2D = isTransform2D;
	position = float3::zero;
	rotation = Quat::identity;
	eulerRotation = float3::zero;
	scale = float3::one;
	localTransform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = localTransform;
	parentGlobalTransform = float4x4::identity;

	if (is2D) pivot = float2(.5f, .5f);
}

Component_Transform::Component_Transform(float3 positionTransform, Quat rotationTransform, float3 scaleTransform, bool isTransform2D) : Component(ComponentType::TRANSFORM)
{
	is2D = isTransform2D;
	position = positionTransform;
	rotation = rotationTransform;
	eulerRotation = rotationTransform.ToEulerXYZ() * RADTODEG;
	scale = scaleTransform;
	localTransform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = localTransform;
	parentGlobalTransform = float4x4::identity;

	if (is2D) pivot = float2(.5f, .5f);
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

		float rot[3] = { eulerRotation.x, eulerRotation.y, eulerRotation.z };
		if (ImGui::DragFloat3("Rotation", rot, 0.1f, -360.0f, 360.0f))
		{
			SetRotation(rot[0], rot[1], rot[2]);
			UpdateGlobalTransform();
			ownerGameObject->UpdateChildrenTransforms();
		}

		float scl[3] = { scale.x, scale.y, scale.z };
		if (ImGui::DragFloat3("Scale", scl, 0.01f, -10000.0f, 10000.0f))
		{
			SetScale(scl[0], scl[1], scl[2]);
			UpdateGlobalTransform();
			ownerGameObject->UpdateChildrenTransforms();
		};

		if (is2D)
		{
			float piv[2] = { pivot.x, pivot.y };
			if (ImGui::DragFloat2("Pivot", piv, 0.01f, 0.0f, 1.0f))
			{
				SetPivot(piv[0], piv[1]);
				UpdateGlobalTransform();
				ownerGameObject->UpdateChildrenTransforms();
				Component_Graphic* compgrph = ownerGameObject->GetComponent<Component_Graphic>();
				if(compgrph != nullptr) ownerGameObject->GetComponent<Component_Graphic>()->RegenerateMesh(this);
			};
		}

		if (ImGui::Button("Reset"))
		{
			ResetTransform();
		}

		ImGui::Spacing();
	}

}

bool Component_Transform::IsTransform2D()
{
	return is2D;
}

void Component_Transform::Save(JsonArray& saveArray)
{
	JsonObj saveObject;
	saveObject.AddInt("Type", type);
	saveObject.AddFloat3("Position", position);
	saveObject.AddQuaternion("Rotation", rotation);
	saveObject.AddFloat3("Scale", scale);
	saveArray.AddObject(saveObject);
}

void Component_Transform::Load(JsonObj& loadObject)
{
	position = loadObject.GetFloat3("Position");
	rotation = loadObject.GetQuaternion("Rotation");
	scale = loadObject.GetFloat3("Scale");

	UpdateEulerRotation();
	UpdateGlobalTransform();
}

void Component_Transform::Set(float4x4 transform)
{
	localTransform = transform;
}

float4x4 Component_Transform::GetLocalTransform()
{
	return localTransform;
}

float4x4 Component_Transform::GetGlobalTransform()
{
	return globalTransform;
}

void Component_Transform::SetGlobalTransform(float4x4 newTransform)
{
	globalTransform = newTransform;
	float4x4 inverseParentGlobal = parentGlobalTransform;
	inverseParentGlobal.Inverse();
	localTransform = inverseParentGlobal * globalTransform;
	UpdateTRS();
	ownerGameObject->UpdateChildrenTransforms();
}

void Component_Transform::UpdateLocalTransform()
{
	localTransform = float4x4::FromTRS(position, rotation, scale);
	UpdateEulerRotation();
}

void Component_Transform::UpdateTRS()
{
	localTransform.Decompose(position, rotation, scale);
	UpdateEulerRotation();
}

void Component_Transform::UpdateGlobalTransform()
{
	UpdateLocalTransform();
	globalTransform = parentGlobalTransform * localTransform;
}

void Component_Transform::UpdateGlobalTransform(float4x4 newParentGlobalTransform)
{
	UpdateLocalTransform();
	parentGlobalTransform = newParentGlobalTransform;
	globalTransform = parentGlobalTransform * localTransform;
}

void Component_Transform::ChangeParentTransform(float4x4 newParentGlobalTransform)
{
	parentGlobalTransform = newParentGlobalTransform;
	newParentGlobalTransform.Inverse();
	localTransform =  newParentGlobalTransform * globalTransform;
	UpdateTRS();
	globalTransform = parentGlobalTransform * localTransform;
}

void Component_Transform::ResetTransform()
{
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;
	eulerRotation = float3::zero;

	UpdateGlobalTransform();
	ownerGameObject->UpdateChildrenTransforms();
	
}

float3 Component_Transform::GetPosition()
{
	return position;
}

void Component_Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	UpdateLocalTransform();
}

Quat Component_Transform::GetRotation()
{
	return rotation;
}

float3 Component_Transform::GetEulerRotation()
{
	return eulerRotation;
}

void Component_Transform::SetPosition(float3 newPosition)
{
	position = newPosition;
	UpdateGlobalTransform();
}

void Component_Transform::SetRotation(float x, float y, float z)
{
	eulerRotation = float3(x, y, z);
	rotation = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
	UpdateGlobalTransform();
}

void Component_Transform::SetRotation(Quat newRotation)
{
	rotation = newRotation;
	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	UpdateGlobalTransform();
}

void Component_Transform::SetRotation(float x, float y, float z, float w)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	rotation.w = w;

	UpdateGlobalTransform();
}

void Component_Transform::UpdateEulerRotation()
{
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation *= RADTODEG;
}

float3 Component_Transform::GetScale()
{
	return scale;
}

void Component_Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	UpdateGlobalTransform();
}

void Component_Transform::SetScale(float3 newScale)
{
	scale = newScale;

	UpdateGlobalTransform();
}

float2 Component_Transform::GetPivot()
{
	return pivot;
}

void Component_Transform::SetPivot(float2 newPivot)
{
	if (newPivot.x > 1.0f) newPivot.x = 1.0f;
	if (newPivot.y > 1.0f) newPivot.y = 1.0f;
	if (newPivot.x < 0.0f) newPivot.x = 0.0f;
	if (newPivot.y < 0.0f) newPivot.y = 0.0f;
	pivot = newPivot;
}

void Component_Transform::SetPivot(float x, float y)
{
	float2 newPivot = float2(x, y);
	if (newPivot.x > 1.0f) newPivot.x = 1.0f;
	if (newPivot.y > 1.0f) newPivot.y = 1.0f;
	if (newPivot.x < 0.0f) newPivot.x = 0.0f;
	if (newPivot.y < 0.0f) newPivot.y = 0.0f;
	pivot = newPivot;
}

void Component_Transform::SetProportionalScale(float multiplier)
{
	scale.x = scale.y = scale.z = multiplier;

	UpdateGlobalTransform();
}