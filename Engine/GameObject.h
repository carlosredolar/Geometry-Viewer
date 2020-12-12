#pragma once

#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Component.h"

#include <vector>
#include <string>

#include "MathGeoLib/include/MathGeoLib.h"

class Component_Transform;
class Component_Mesh;

class JsonObj;
class JsonArray;

class GameObject {
public:
	GameObject();
	GameObject(Component_Mesh* mesh);
	~GameObject();

	void Update();
	bool IsVisible();
	void OnGUI();

	void Save(JsonArray& save_array);
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

public: 
	uint UUID = 0;

	bool toDelete;

	OBB obb;
	AABB aabb;

private:
	bool enabled;
	bool isVisible;
	std::string name;
	GameObject* parent;
	Component_Transform* transform;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
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