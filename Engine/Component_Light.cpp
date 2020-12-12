#include "Globals.h"
#include "Component_Light.h"
#include <gl/GL.h>
#include "ImGui/imgui.h"
#include "ModuleJson.h"

Component_Light::Component_Light() : Component(ComponentType::LIGHT), ref(-1), position(0.0f, 0.0f, 0.0f)
{}

Component_Light::Component_Light(GameObject* gameObject) : Component(ComponentType::LIGHT, gameObject), ref(-1), position(0.0f, 0.0f, 0.0f)
{}

Component_Light::~Component_Light()
{
}

void Component_Light::Init()
{
	glLightfv(ref, GL_AMBIENT, &ambient);
	glLightfv(ref, GL_DIFFUSE, &diffuse);
}

void Component_Light::Update()
{
	Render();
}

void Component_Light::SetPos(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Component_Light::Render()
{
	if(enabled)
	{
		float pos[] = {position.x, position.y, position.z, 1.0f};
		glLightfv(ref, GL_POSITION, pos);
	}
}

void Component_Light::OnGUI()
{
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float diffuse4f[4] = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };
		if (ImGui::DragFloat3("Diffuse Color", diffuse4f, 0.1f, 0.0f, 1.0f))
		{
			diffuse.r = diffuse4f[0];
			diffuse.g = diffuse4f[1];
			diffuse.b = diffuse4f[2];
			diffuse.a = diffuse4f[3];
		}

		float ambient4f[4] = { ambient.r, ambient.g, ambient.b, ambient.a };
		if (ImGui::DragFloat3("Ambient Color", ambient4f, 0.1f, 0.0f, 1.0f))
		{
			ambient.r = ambient4f[0];
			ambient.g = ambient4f[1];
			ambient.b = ambient4f[2];
			ambient.a = ambient4f[3];
		}
	}
}

void Component_Light::Save(JsonArray& save_array)
{
	JsonObj save_object;

	save_object.AddString("name", name.c_str());
	save_object.AddInt("Type", type);
	save_object.AddFloat3("position", float3(position.x, position.y, position.z));
	save_object.AddColor("diffuse", diffuse);
	save_object.AddColor("ambient", ambient);

	save_array.AddObject(save_object);
}

void Component_Light::Load(JsonObj& load_object)
{
	name = load_object.GetString("name", "Light");
	float3 fposition = load_object.GetFloat3("position");
	position = vec3(fposition.x, fposition.y, fposition.z);
	diffuse = load_object.GetColor("diffuse");
	ambient = load_object.GetColor("ambient");
}

void Component_Light::Active(bool active)
{
	enabled = active;

	if(enabled)
		glEnable(ref);
	else
		glDisable(ref);
}