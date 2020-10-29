#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include "Component.h"

class GameObject
{
public:
	//Constructor
	GameObject(const char* name, GameObject* parent, bool enabled = true);

	//Destructor
	~GameObject();

	void Update(); //Update childs and components

	void Enable(); //Enable GameObject

	void Disable(); //Disable GameObject

	bool IsEnabled(); //Return if enabled

	GameObject* const GetParent() const; //Get it's parent

	void AddGameObjectAsChild(GameObject* gameObject); //Add another GameObject as a child

	void ChangeParent(GameObject* newParent); //Change parent

	std::vector<GameObject*>* const GetChilds(); //Get a pointer to this game object childs vector

	//Separate child from this game object
	void SeparateChild(GameObject* child);

	//Delete child
	void DeleteChild(GameObject* child);

	//Send this game object childs to this parent
	void GetChildsNewParent();

	//Add a component to this game object
	Component* CreateComponent(Component::COMPONENT_TYPE type);

	//Check and add component
	void CheckAddComponent(Component* new_comp);

	//Get a pointer to this game object components vector
	std::vector<Component*>* const GetComponents();

	//Get a reference to this game object name
	const char* const GetName();

	//Change game object's name. Passed by reference
	void ChangeName(char* new_name);

private:
	GameObject*					parent;
	const char*					name;
	bool						enabled;
	std::vector<Component*>		components;
	std::vector<GameObject*>	childs;
};

#endif // !__GAMEOBJECT_H__