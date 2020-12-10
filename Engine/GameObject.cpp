#include "GameObject.h"
#include "Component.h"
#include "Component_Transform.h"
#include "Component_Mesh.h"
#include "Component_Material.h"
#include "Component_Camera.h"
#include "Libs/ImGui/imgui.h"
#include "ModuleJson.h"
#include "Application.h"

#include "Libs/MathGeoLib/include/MathGeoLib.h"

#include <vector>

GameObject::GameObject(const char* _name, GameObject* _parent, bool _enabled) :name(_name), parent(_parent), enabled(_enabled)
{
	UUID = LCG().Int();
	name = _name;
	parent = _parent;
	enabled = _enabled;
	to_delete = false;
	visible = false;
}

//GameObject::GameObject(Component_Mesh* mesh) : GameObject()
//{
//	SetName(mesh->name);
//	AddComponent((Component*)mesh);
//}

GameObject::~GameObject()
{
	parent = nullptr;

	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}

	components.clear();
	childs.clear();
	name.clear();
	UUID = 0;
}

void GameObject::Update()
{
	if (enabled)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			//Update Components
			if (components[i]->IsEnabled())
			{
				if (components[i]->GetType() == ComponentType::MESH)
				{
					Component_Mesh* mesh = (Component_Mesh*)components[i];
					CreateAABB();

					if (App->renderer3D->IsInsideCameraView(aABB))
						mesh->Update();
				}
				else
				{
					components[i]->Update();
				}
			}
		}

		//Update Children
		for (size_t i = 0; i < childs.size(); i++)
		{
			childs[i]->Update();
		}
		GetComponent<Component_Transform>()->UpdateGlobalTransform(parent->GetComponent<Component_Transform>()->GetGlobalTransform());
	}
}

void GameObject::Enable()
{
	if (!IsEnabled())
	{
		enabled = true;

		//Enable components
		for (int i = 0; 0 < components.size(); i++) {
			components[i]->Enable();
		}

		//Enable children
		for (int i = 0; 0 < childs.size(); i++) {
			childs[i]->Enable();
		}
	}

}

void GameObject::Disable()
{
	if (IsEnabled())
	{
		enabled = false;

		//Disable components
		for (int i = 0; 0 < components.size(); i++) {
			components[i]->Disable();
		}

		//Disable children
		for (int i = 0; 0 < childs.size(); i++) {
			childs[i]->Disable();
		}
	}
}

bool GameObject::IsEnabled()
{
	return enabled;
}

void GameObject::OnGUI()
{
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SameLine();

	static char buf[64] = "Name";
	strcpy(buf, name.c_str());
	if (ImGui::InputText("", &buf[0], IM_ARRAYSIZE(buf))) {}

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnGUI();
	}

	if (ImGui::CollapsingHeader("Debugging Information"))
	{
		if (parent != nullptr)
			ImGui::Text("Parent: %s", parent->GetName());
		else
			ImGui::Text("No parent");

		ImGui::Text("UUID: %d", UUID);
	}
}

void GameObject::Save(JsonArray & save_array)
{
	JsonObj save_object;

	save_object.AddInt("UUID", UUID);

	if (parent != nullptr)
		save_object.AddInt("Parent UUID", parent->UUID);
	else
		save_object.AddInt("Parent UUID", 0);

	save_object.AddString("Name", name.c_str());

	JsonArray componentsSave = save_object.AddArray("Components");

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Save(componentsSave);
	}

	save_array.AddObject(save_object);

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->Save(save_array);
	}
}

uint GameObject::Load(JsonObj* object)
{
	UUID = object->GetInt("UUID");
	name = object->GetString("Name", "No Name");
	uint parentUUID = object->GetInt("Parent UUID");

	JsonArray componentsArray = object->GetArray("Components");

	for (size_t i = 0; i < componentsArray.Size(); i++)
	{
		JsonObj componentObject = componentsArray.GetObjectAt(i);
		Component* component = CreateComponent((ComponentType)componentObject.GetInt("Type"));
		component->Load(componentObject);
	}

	return parentUUID;
}

uint GameObject::LoadNodeData(JsonObj* object)
{
	UUID = object->GetInt("UUID");
	name = object->GetString("Name", "No Name");
	uint parentUUID = object->GetInt("Parent UUID");

	float3 position = object->GetFloat3("Position");
	Quat rotation = object->GetQuaternion("Rotation");
	float3 scale = object->GetFloat3("Scale");

	GetComponent<Component_Transform>()->SetTransform(position, rotation, scale);

	int meshID = object->GetInt("Mesh", -1);
	if (meshID != -1) {
		Component_Mesh* mesh = (Component_Mesh*)CreateComponent(ComponentType::MESH);
		mesh->SetResourceUID(meshID);
	}

	int materialID = object->GetInt("Material", -1);
	if (materialID != -1) {
		Component_Material* material = (Component_Material*)CreateComponent(ComponentType::MATERIAL);
		material->SetResourceUID(materialID);
	}

	return parentUUID;
}

//Component* GameObject::GetComponent(ComponentType component)
//{
//	for (size_t i = 0; i < components.size(); i++)
//	{
//		if (components[i]->GetType() == component)
//		{
//			return components[i];
//		}
//	}
//
//	return nullptr;
//}

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

std::vector<GameObject*>* GameObject::GetChilds()
{
	return &childs;
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* component = nullptr;
	Component_Transform* transform = GetComponent<Component_Transform>();
	switch (type)
	{
	case TRANSFORM:
		if (transform != nullptr)
		{
			RemoveComponent(transform);
		}

		transform = new Component_Transform(this);
		component = transform;
		break;
	case MESH:
		component = new Component_Mesh(this);
		break;
	case MATERIAL:
		component = new Component_Material(this);
		break;
	case CAMERA:
		component = new Component_Camera(this);
		break;
	case LIGHT:
		component = new Light(this);
		break;
	default:
		break;
	}

	components.push_back(component);

	return component;
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
	component->SetOwnerGameObject(this);
}

bool GameObject::RemoveComponent(Component * component)
{
	bool ret = false;

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i] == component) {
			delete components[i];
			components.erase(components.begin() + i);
			component = nullptr;
			ret = true;
		}
	}

	return ret;
}

const char* GameObject::GetName() { return name.c_str(); }

void GameObject::ChangeName(const char* g_name)
{
	name = g_name;
}

//void GameObject::SetTransform(Component_Transform g_transform)
//{
//	localTransform->Set(g_transform.GetLocalTransform());
//	localTransform->UpdateLocalTransform();
//	memcpy(transform, &g_transform, sizeof(g_transform));
//}

//Component_Transform* GameObject::GetTransform()
//{
//	return transform;
//}

AABB GameObject::GetAABB()
{
	return aABB;
}

bool GameObject::IsVisible()
{
	return visible;
}

void GameObject::AddChild(GameObject * child)
{
	if (child != nullptr)
		childs.push_back(child);

	child->ChangeParent(this);
}

int GameObject::GetChildrenAmount()
{
	return childs.size();
}

GameObject* GameObject::GetChildAt(int index)
{
	return childs[index];
}

GameObject* GameObject::GetParent()
{
	return parent;
}

void GameObject::ChangeParent(GameObject * g_parent)
{
	parent = g_parent;
}

void GameObject::Reparent(GameObject * newParent)
{
	if (newParent != nullptr)
	{
		parent->RemoveChild(this);
		parent = newParent;
		newParent->AddChild(this);
	}
}

bool GameObject::RemoveChild(GameObject * gameObject)
{
	bool ret = false;
	for (size_t i = 0; i < childs.size(); i++)
	{
		if (childs[i] == gameObject)
		{
			childs.erase(childs.begin() + i);
			ret = true;
		}
	}
	return ret;
}

void GameObject::DeleteAllChilds()
{
	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->DeleteAllChilds();
		childs[i] = nullptr;
	}

	this->~GameObject();
}

void GameObject::CreateAABB()
{
	OBB oBB = GetComponent<Component_Mesh>()->GetAABB();
	oBB.Transform(GetComponent<Component_Transform>()->GetGlobalTransform());

	aABB.SetNegativeInfinity();
	aABB.Enclose(oBB);

	float3 cornerPoints[8];
	aABB.GetCornerPoints(cornerPoints);
	App->renderer3D->DrawAABB(cornerPoints);
}