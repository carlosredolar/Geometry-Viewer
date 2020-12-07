#include "Application.h"
#include "GameObject.h"
#include "Component_Mesh.h"
#include "Component_Transform.h"
#include "Component_Texture.h"
#include "Component_Camera.h"

#include "Component.h"



GameObject::GameObject(const char* name, GameObject* parent, bool enabled) :name(name), parent(parent), enabled(enabled) { selected = false; }

GameObject::~GameObject()
{
	//std::vector<Component*>::iterator component = components.begin();

	//for (; component != components.end(); ++component) {
	//	delete (*component);
	//}

	//components.clear();

	//std::vector<GameObject*>::iterator gameObject = childs.begin();

	//for (; gameObject != childs.end(); ++gameObject) {
	//	delete (*gameObject);
	//}

	//childs.clear();

	//parent = nullptr;
	//name.clear();
}

void GameObject::DeleteComponents()
{
	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component) {
		delete (*component);
	}

	components.clear();

	std::vector<GameObject*>::iterator gameObject = childs.begin();

	for (; gameObject != childs.end(); ++gameObject) {
		(*gameObject)->DeleteComponents();
		delete (*gameObject);
	}

	childs.clear();
	parent->DeleteChild(this);
	parent = nullptr;
	name.clear();
}

void GameObject::Update()
{
	if (IsEnabled()) 
	{
		//Update components
		std::vector<Component*>::iterator component = components.begin();

		for (; component != components.end(); ++component) {
			if((*component)->IsEnabled()) (*component)->Update();

			/*if ((*component != nullptr) && (*component)->GetType() == COMPONENT_TYPE::MESH) {
				Component_Mesh* mesh = (Component_Mesh*)*component;
				_OBB = mesh->GetAABB();
				_OBB.Transform(GetComponent<Component_Transform>()->GetGlobalTransform());

				_AABB.SetNegativeInfinity();
				_AABB.Enclose(_OBB);
				mesh->GenerateAABB();

				float3 cornerPoints[8];
				_AABB.GetCornerPoints(cornerPoints);
				App->renderer3D->DrawAABB(cornerPoints);
			}*/
		}

		//Update childs
		std::vector<GameObject*>::iterator gameObject = childs.begin();

		for (; gameObject != childs.end(); ++gameObject) {
			if ((*gameObject)->IsEnabled()) (*gameObject)->Update();

			(*gameObject)->GetComponent<Component_Transform>()->UpdateGlobalTransform(GetComponent<Component_Transform>()->GetGlobalTransform());
		}

		
	}
}

void GameObject::Enable()
{
	if (!IsEnabled()) 
	{
		enabled = true;

		//Enable components
		std::vector<Component*>::iterator component = components.begin();

		for (; component != components.end(); ++component) {
			(*component)->Enable();
		}

		//Enable children
		std::vector<GameObject*>::iterator gameObject = childs.begin();

		for (; gameObject != childs.end(); ++gameObject) {
			(*gameObject)->Enable();
		}
	}
	
}

void GameObject::Disable()
{
	if (IsEnabled())
	{
		enabled = false;

		//Disable components
		std::vector<Component*>::iterator component = components.begin();

		for (; component != components.end(); ++component) {
			(*component)->Disable();
		}

		//Disable children
		std::vector<GameObject*>::iterator gameObject = childs.begin();

		for (; gameObject != childs.end(); ++gameObject) {
			(*gameObject)->Disable();
		}
	}
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

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component_Texture* componentTexture = nullptr;
	Component_Mesh* componentMesh = nullptr;
	Component_Camera* componentCamera = nullptr;
	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		components.push_back(new Component_Transform(this));
		break;
	case COMPONENT_TYPE::TEXTURE:
		componentTexture = new Component_Texture(this);
		CheckAddComponent(componentTexture);
		return componentTexture;
		break;
	case COMPONENT_TYPE::MESH:
		componentMesh = new Component_Mesh(this);
		CheckAddComponent(componentMesh);
		return componentMesh;
		break;
	case COMPONENT_TYPE::CAMERA:
		componentCamera = new Component_Camera();
		break;
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
		while (prnt == nullptr && prnt->name != "Scene")
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
