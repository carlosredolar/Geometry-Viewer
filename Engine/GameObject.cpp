#include "Application.h"
#include "GameObject.h"
#include "Component_Mesh.h"
#include "Component_Transform.h"
#include "Component_Material.h"
#include "Component.h"
#include "ImGui/imgui.h"

GameObject::GameObject(const char* name, GameObject* parent, bool enabled) :name(name), parent(parent), enabled(enabled) { selected = false; to_delete = false; }

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
		}

		//Update childs
		std::vector<GameObject*>::iterator gameObject = childs.begin();

		for (; gameObject != childs.end(); ++gameObject) {
			if ((*gameObject)->IsEnabled()) (*gameObject)->Update();

			(*gameObject)->GetComponent<Component_Transform>()->UpdateGlobalTransform(GetComponent<Component_Transform>()->GetGlobalTransform());
		}

		//Draw AABB
		float3 cornerPoints[8];
		aABB.GetCornerPoints(cornerPoints);
		App->renderer3D->DrawAABB(cornerPoints);
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

		//Enable childs
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

		//Disable childs
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

void GameObject::Save(JsonArray& save_array)
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

GameObject* const GameObject::GetParent() const
{
	return parent;
}

void GameObject::CreateAABB()
{
	OBB oBB = GetComponent<Component_Mesh>()->GetAABB();
	oBB.Transform(GetComponent<Component_Transform>()->GetGlobalTransform());

	aABB.SetNegativeInfinity();
	aABB.Enclose(oBB);
}

AABB GameObject::GetAABB()
{
	return aABB;
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

void GameObject::ChangeParent(GameObject * newparent)
{
	parent = newparent;
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

void GameObject::DeleteAllChilds()
{
	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->DeleteAllChilds();
		childs[i] = nullptr;
	}

	//this->~GameObject();
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component_Material* componentTexture = nullptr;
	Component_Mesh* componentMesh = nullptr;
	switch (type)
	{
	case ComponentType::TRANSFORM:
		components.push_back(new Component_Transform(this));
		break;
	case ComponentType::MATERIAL:
		componentTexture = new Component_Material(this);
		CheckAddComponent(componentTexture);
		return componentTexture;
		break;
	case ComponentType::MESH:
		componentMesh = new Component_Mesh(this);
		CheckAddComponent(componentMesh);
		CreateAABB();
		return componentMesh;
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

void GameObject::ChangeName(const char* new_name)
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
