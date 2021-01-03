#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Mesh;
class JsonArray;

class Component_Transform : public Component {
public:
	Component_Transform(bool isTransform2D = false);
	Component_Transform(float3 positionTransform, Quat rotationTransform, float3 scaleTransform, bool isTransform2D = false);
	~Component_Transform();
	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void Set(float4x4 transform);

	float4x4 GetLocalTransform();
	float4x4 GetGlobalTransform();
	void SetGlobalTransform(float4x4 newTransform);

	void UpdateLocalTransform();
	void UpdateTRS();
	void UpdateGlobalTransform();
	void UpdateGlobalTransform(float4x4 newParentGlobalTransform);
	void ChangeParentTransform(float4x4 newParentGlobalTransform);
	void ResetTransform();

	float3 GetPosition();
	void SetPosition(float x, float y, float z);
	void SetPosition(float3 newPosition);

	Quat GetRotation();
	void SetRotation(float x, float y, float z);
	void SetRotation(Quat newRotation);
	void SetRotation(float i, float j, float k, float w);
	void UpdateEulerRotation();

	float3 GetScale();
	void SetScale(float x, float y, float z);
	void SetScale(float3 newScale);
	void SetProportionalScale(float multiplier);

	float2 GetPivot();
	void SetPivot(float x, float y);
	void SetPivot(float2 newPivot);

	bool IsTransform2D();
	

private:
	bool is2D = false;

	float4x4 localTransform = float4x4::identity;
	float4x4 globalTransform = float4x4::identity;
	float4x4 parentGlobalTransform = float4x4::identity;

	float3 position;
	Quat rotation;
	float3 scale;
	float3 eulerRotation;
	float2 pivot;
};

#endif //__TRANSFORM_H__