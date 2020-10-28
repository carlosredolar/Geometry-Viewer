#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <vector>
#include "Component.h"

class GameObject
{
public:
	//Constructor
	GameObject(char* name, GameObject* parent, bool active = true);

	//Destructor
	~GameObject();

	void Update(); //Update childs and components

	void Enable(); //Enable GameObject

	void Disable(); //Disable GameObject

	bool IsEnabled(); //Return if enabled


	GameObject* const GetParent() const; //Get it's parent

	void AddGameObjectAsChild(GameObject* game_object); //Add another GameObject as a child

	void ChangeParent(GameObject* new_parent); //Change parent

	std::vector<GameObject*>* const GetChilds(); //Get a pointer to this game object childs vector

	//Erasing a child from this game object but not deleting it
	void EraseChildPointer(GameObject* child);

	//Completly delete a child. Send a true boolean to delete all childs!
	void DeleteChild(GameObject* child);

	//Send this game object childs to this parent
	void GetChildsNewParent();


	//Add a component to this game object. Enter type!
	void CreateComponent(Component::COMPONENT_TYPE type);

	//Check and add component
	void CheckAddComponent(Component* new_comp);

	//Get a pointer to this game object components vector
	std::vector<Component*>* const GetComponents();


	//Get a reference to this game object name
	std::string& const GetName();

	//Change game object's name. Passed by reference
	void ChangeName(std::string& new_name);

private:
	GameObject* parent;
	bool						active;
	std::string					name;
	std::vector<Component*>		components;
	std::vector<GameObject*>	childs;
};

#endif // !__GAMEOBJECT_H__