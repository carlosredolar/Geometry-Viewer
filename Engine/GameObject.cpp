#include "GameObject.h"
#include "Component.h"
#include "Component_Transform.h"
#include "Component_Mesh.h"
#include "Component_Material.h"
#include "Component_Camera.h"
#include "Component_Light.h"
#include "Component_Canvas.h"
#include "Component_Image.h"
#include "Component_Button.h"
#include "Component_CanvasRenderer.h"
#include "Libs/ImGui/imgui.h"
#include "ModuleJson.h"
#include "Application.h"
#include "ModuleGui.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Libs/Glew/include/glew.h"

#include <vector>

GameObject::GameObject() : enabled(true), name("Empty Game Object"), parent(nullptr), toDelete(false), transform(nullptr), isVisible(false)
{
	transform = (Component_Transform*)AddComponent(TRANSFORM);
	UUID = LCG().Int();
}

GameObject::GameObject(Component_Mesh* mesh) : GameObject()
{
	SetName(mesh->name);
	AddComponent((Component*)mesh);
}

GameObject::GameObject(Component_Transform* trans, const char* name) : GameObject()
{
	SetName(name);
	DeleteComponent(transform);
	AddComponent((Component*)trans);
	transform = trans;
}

GameObject::~GameObject()
{
	parent = nullptr;

	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}

	transform = nullptr;
	components.clear();
	children.clear();
	name.clear();
	UUID = 0;
}

void GameObject::Update()
{
	if (enabled)
	{
		//if (transform->IsTransform2D())
		//{
		//	glDisable(GL_DEPTH_TEST);
		//	glMatrixMode(GL_PROJECTION);
		//	glPushMatrix();
		//	glLoadIdentity();
		//	glOrtho(0, App->gui->sceneRenderSize.x, App->gui->sceneRenderSize.y, 0, 1, -1);
		//	glMatrixMode(GL_MODELVIEW);
		//	glPushMatrix();
		//	glLoadIdentity();
		//}

		for (size_t i = 0; i < components.size(); i++)
		{
			//Update Components
			if (components[i]->IsEnabled())
			{
				if (components[i]->GetType() == ComponentType::MESH)
				{
					Component_Mesh* mesh = (Component_Mesh*)components[i];

					obb = mesh->GetAABB();
					obb.Transform(transform->GetGlobalTransform());

					aabb.SetNegativeInfinity();
					aabb.Enclose(obb);

					if (App->scene->showBB)
					{
						float3 cornerPoints[8];
						aabb.GetCornerPoints(cornerPoints);

						ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);

						App->renderer3D->DrawAABB(cornerPoints, color);
					}

					if (App->camera->GetCamera()->CheckBBOnCamera(aabb))
					{
						if (App->renderer3D->GetMainCamera()->CheckBBOnCamera(aabb) || !App->renderer3D->cameraCulling)
						{
							mesh->Update();
						}
					}
				}
				else
				{
					components[i]->Update();
				}
			}
		}

		//if (transform->IsTransform2D())
		//{
		//	glMatrixMode(GL_PROJECTION);
		//	glPopMatrix();
		//	glMatrixMode(GL_MODELVIEW);
		//	glPopMatrix();
		//	glEnable(GL_DEPTH_TEST);
		//}

		//Update Children
		for (size_t i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}
	}
}

bool GameObject::IsVisible()
{
	return isVisible;
}

void GameObject::OnGUI()
{
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SameLine();

	static char buf[64] = "Name";
	strcpy(buf, name.c_str());
	if (ImGui::InputText("", &buf[0], IM_ARRAYSIZE(buf))){}

	ImGui::SameLine();

	ImGui::Text("Info");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		if (parent != nullptr)
			ImGui::Text("Parent: %s", parent->GetName());
		else
			ImGui::Text("No parent");

		ImGui::Text("UUID: %d", UUID);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnGUI();
	}
}

void GameObject::Save(JsonArray& saveArray)
{
	JsonObj saveObject;

	saveObject.AddInt("UUID", UUID);

	if(parent != nullptr)
		saveObject.AddInt("Parent UUID",parent->UUID);
	else 
		saveObject.AddInt("Parent UUID", 0);

	saveObject.AddString("Name", name.c_str());

	JsonArray componentsSave = saveObject.AddArray("Components");

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Save(componentsSave);
	}

	saveArray.AddObject(saveObject);

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->Save(saveArray);
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
		Component* component = AddComponent((ComponentType)componentObject.GetInt("Type"));
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
	transform->SetPosition(position);

	Quat rotation = object->GetQuaternion("Rotation");
	transform->SetRotation(rotation);

	float3 scale = object->GetFloat3("Scale");
	transform->SetScale(scale);

	int meshID = object->GetInt("Mesh", -1);
	if (meshID != -1) {
		Component_Mesh* mesh = (Component_Mesh*)AddComponent(ComponentType::MESH);
		mesh->SetResourceUID(meshID);
	}

	int materialID = object->GetInt("Material", -1);
	if (materialID != -1) {
		Component_Material* material = (Component_Material*)AddComponent(ComponentType::MATERIAL);
		material->SetResourceUID(materialID);
	}

	return parentUUID;
}

const char* GameObject::GetName() 
{
	return name.c_str(); 
}

void GameObject::SetName(const char* nameGameObject) 
{
	name = nameGameObject;
}

void GameObject::SetTransform(Component_Transform g_transform)
{
	memcpy(transform, &g_transform, sizeof(g_transform));
}

Component_Transform* GameObject::GetTransform()
{
	return transform;
}

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;

	switch (type)
	{
	case TRANSFORM:
		if (transform != nullptr)
		{
			DeleteComponent(transform);
		}

		transform = new Component_Transform();
		component = transform;
		break;
	case MESH:
		component = new Component_Mesh();
		break;
	case MATERIAL:
		component = new Component_Material(this);
		break;
	case CAMERA:
		component = new Component_Camera(this);
		break;
	case LIGHT:
		component = new Component_Light(this);
		break;
	case CANVAS:
		component = new Component_Canvas(this);
		break;
	case IMAGE:
		component = new Component_Image(this);
		break;
	case BUTTON:
		component = new Component_Button(this);
		break;
	case CANVASRENDERER:
		component = new Component_CanvasRenderer(this);
		break;
	default:
		break;
	}

	if (component != nullptr)
	{
		component->SetGameObject(this);
		components.push_back(component);
	}
	else
	{
		LOG_ERROR("Component was nullptr when creating a new one.");
	}

	return component;
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
	component->SetGameObject(this);
}

bool GameObject::DeleteComponent(Component* component)
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

void GameObject::AddChild(GameObject* child)
{
	if (child != nullptr)
	{
		bool ret = true;
		for (size_t i = 0; i < children.size(); i++)
		{
			if (children[i] == child)
			{
				ret = false;
			}
		}

		if (ret)
		{
			children.push_back(child);
			child->SetParent(this);
		}
	}
}

int GameObject::GetChildrenAmount()
{
	return children.size();
}

GameObject* GameObject::GetChildAt(int index)
{
	return children[index];
}

GameObject* GameObject::GetParent()
{
	return parent;
}

void GameObject::SetParent(GameObject* parentGameObject)
{
	parent = parentGameObject;
}

void GameObject::ChangeParent(GameObject* newParent)
{
	if (newParent != nullptr) 
	{
		parent->RemoveChild(this);
		parent = newParent;
		newParent->AddChild(this);
		transform->ChangeParentTransform(newParent->GetTransform()->GetGlobalTransform());
	}
}

bool GameObject::RemoveChild(GameObject* gameObject)
{
	bool ret = false;
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i] == gameObject)
		{
			children.erase(children.begin() + i);
			ret = true;
		}
	}
	return ret;
}

void GameObject::DeleteAllChildren()
{
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->DeleteAllChildren();
		children[i] = nullptr;
	}

	this->~GameObject();
}

void GameObject::UpdateChildrenTransforms()
{
	transform->UpdateGlobalTransform();

	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i]->GetTransform() != nullptr)
		{
			children[i]->GetTransform()->UpdateGlobalTransform(transform->GetGlobalTransform());
			children[i]->UpdateChildrenTransforms();
		}
	}
}

AABB GameObject::GetAABB()
{
	return aabb;
}