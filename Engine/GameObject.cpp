#include "GameObject.h"
#include "Component_Mesh.h"
#include "Component_Transform.h"
#include "Component.h"

GameObject::GameObject(const char* name, GameObject* parent, bool enabled) :name(name), parent(parent), enabled(enabled) {}

GameObject::~GameObject()
{

}

void GameObject::Update()
{
	//Update components
	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component) {
		(*component)->Update();
	}

	//Update childs
	std::vector<GameObject*>::iterator gameObject = childs.begin();

	for (; gameObject != childs.end(); ++gameObject) {
		(*gameObject)->Update();
	}
}

void GameObject::Enable()
{
	enabled = true;
}

void GameObject::Disable()
{
	enabled = false;
}

bool GameObject::IsEnabled()
{
	return enabled;
}

GameObject* const GameObject::GetParent() const
{
	return parent;
}

void GameObject::AddGameObjectAsChild(GameObject * gameObject)
{
	if(gameObject->parent != this)
	{
		gameObject->parent->SeparateChild(gameObject);
		gameObject->ChangeParent(this);
	}
	
	childs.push_back(gameObject);
}

void GameObject::ChangeParent(GameObject * new_parent)
{
	parent = new_parent;
}

std::vector<GameObject*>* const GameObject::GetChilds()
{
	return &childs;
}

void GameObject::SeparateChild(GameObject * child)
{
	std::vector<GameObject*>::iterator it;
	if (child != nullptr) {
		for (it = childs.begin(); it != childs.end(); ++it) {
			if (*it == child)
			{
				childs.erase(it);
				return;
			}
		}
	}
}

void GameObject::DeleteChild(GameObject * child)
{
	std::vector<GameObject*>::iterator it;
	if (child != nullptr) {
		for (it = childs.begin(); it != childs.end(); ++it) {
			if (*it == child)
			{
				(*it)->GetChildsNewParent();
				childs.erase(it);
				delete* it;
				return;
			}
		}
	}
}

Component* GameObject::CreateComponent(Component::COMPONENT_TYPE type)
{
	switch (type)
	{
	case Component::COMPONENT_TYPE::TRANSFORM:
		components.push_back(new Component_Transform(this));
		break;
	case Component::COMPONENT_TYPE::MESH:
		Component_Mesh* componentMesh = new Component_Mesh(this);
		CheckAddComponent(componentMesh);
		return componentMesh;
		break;
	//case Component::COMPONENT_TYPE::MATERIAL:
	//	components.push_back(new Component_Material(type));
	//	break;
	}
}

void GameObject::CheckAddComponent(Component * newComp)
{
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		if (*it != nullptr && *it == newComp)return;
	}
	components.push_back(newComp);
}

std::vector<Component*>* const GameObject::GetComponents()
{
	return &components;
}

const char* const GameObject::GetName()
{
	return name.c_str();
}

void GameObject::ChangeName(char* new_name)
{
	name = new_name;
}

void GameObject::GetChildsNewParent()
{
	GameObject* prnt;
	if (parent != nullptr)
		prnt = parent;
	else
	{
		prnt = parent->GetParent();
		while (prnt == nullptr && prnt->name != "root")
		{
			prnt = prnt->GetParent();
		}
	}
	std::vector<GameObject*>::iterator it = childs.begin();
	for (; it != childs.end(); ++it)
	{
		prnt->AddGameObjectAsChild((*it));
	}
	childs.clear();
}
