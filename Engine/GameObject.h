#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include "Component.h"
#include <string>

#include "MathGeoLib/include/MathGeoLib.h"

class GameObject
{
public:
	//Constructor
	GameObject(const char* _name, GameObject* _parent, bool _enabled = true);

	//Destructor
	~GameObject();

	void Update(); //Update childs and components
	void Enable(); //Enable GameObject
	void Disable(); //Disable GameObject
	bool IsEnabled(); //Return if enabled
	bool IsVisible();

	void OnGUI();

	void Save(JsonArray& save_array);
	uint Load(JsonObj* object);
	uint LoadNodeData(JsonObj* object);

	GameObject* GetParent(); //Get it's parent
	void AddChild(GameObject* gameObject); //Add another GameObject as a child
	void ChangeParent(GameObject* newParent); //Change parent
	void Reparent(GameObject* newParent);

	//std::vector<GameObject*>* const GetChilds(); //Get a pointer to this game object childs vector
	int GetChildrenAmount();
	GameObject* GetChildAt(int index);
	bool RemoveChild(GameObject* gameObject);
	void DeleteAllChilds();
	std::vector<GameObject*>* GetChilds();


	//Add a component to this game object
	Component* CreateComponent(ComponentType type);

	//Check and add component
	void AddComponent(Component* new_comp);

	//Get a pointer to this game object components vector
	std::vector<Component*> GetComponents();

	//Get the component between <>
	template<typename Type>
	Type* GetComponent();

	//Get a reference to this game object name
	const char* GetName();
	AABB GetAABB();
	void CreateAABB();

	//Change game object's name. Passed by reference
	void ChangeName(const char* new_name);

	bool RemoveComponent(Component* component);

	uint UUID;

	bool to_delete;
	bool selected;

private:

	GameObject*					parent;
	std::string					name;
	bool						enabled;
	bool						visible;
	std::vector<Component*>		components;
	std::vector<GameObject*>	childs;

	AABB aABB;
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