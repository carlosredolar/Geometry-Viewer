#pragma once

#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Component.h"

#include <vector>
#include <string>

#include "Libs/MathGeoLib/include/MathGeoLib.h"

class Component_Transform;
class Component_Mesh;

class JsonObj;
class JsonArray;

class GameObject {
public:
	GameObject();
	GameObject(Component_Mesh* mesh);
	GameObject(Component_Transform* transform, const char* name = "Empty Game Object");
	~GameObject();

	void Update();
	bool IsVisible();
	void OnGUI();

	void Save(JsonArray& saveArray);
	uint Load(JsonObj* object);
	uint LoadNodeData(JsonObj* object);

	//GameObject functions
	void SetName(const char* nameGameObject);
	const char* GetName();
	void SetTransform(Component_Transform transform);
	Component_Transform* GetTransform();

	//Component functions
	std::vector<Component*> GetComponents();
	Component* AddComponent(ComponentType type);
	void AddComponent(Component* component);
	bool DeleteComponent(Component* component);
	template<typename Type>
	Type* GetComponent(); 	//Get the component between <>
	
	//Children functions
	void AddChild(GameObject* child);
	int GetChildrenAmount();
	GameObject* GetChildAt(int index);
	bool RemoveChild(GameObject* gameObject);
	void DeleteAllChildren();
	void UpdateChildrenTransforms();

	//Parent functions
	GameObject* GetParent();
	void SetParent(GameObject* parentGameObject);
	void ChangeParent(GameObject* newParent);

	void SetAABB(AABB);
	AABB GetAABB();

public: 
	uint UUID = 0;

	bool toDelete;

	
private:
	bool enabled;
	bool isVisible;
	std::string name;
	GameObject* parent = nullptr;
	Component_Transform* transform;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	OBB obb;
	AABB aabb;
};

template<typename Type>
Type* GameObject::GetComponent()
{
	for (int i = 0; i < components.size(); i++)
	{
		Type* c = dynamic_cast<Type*>(components[i]);
		if (c != nullptr)
			return	c;
	}
	return nullptr;
}

#endif // !__GAMEOBJECT_H__