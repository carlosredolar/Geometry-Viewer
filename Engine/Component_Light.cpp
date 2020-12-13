#include "Globals.h"
#include <gl/GL.h>
#include "Component_Light.h"
#include "ModuleJson.h"
#include "ImGui/imgui.h"

Component_Light::Component_Light() : Component(ComponentType::LIGHT), reference(-1), position(0.0f, 0.0f, 0.0f)
{}

Component_Light::Component_Light(GameObject* gameObject) : Component(ComponentType::LIGHT, gameObject), reference(-1), position(0.0f, 0.0f, 0.0f)
{}

Component_Light::~Component_Light()
{
}

void Component_Light::Init()
{
	glLightfv(reference, GL_AMBIENT, &ambient);
	glLightfv(reference, GL_DIFFUSE, &diffuse);
}

void Component_Light::Update()
{
	Render();
}

void Component_Light::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Component_Light::Enable(bool enable)
{
	enabled = enable;

	if (enabled)
		glEnable(reference);
	else
		glDisable(reference);
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

void Component_Light::Save(JsonArray& saveArray)
{
	JsonObj saveObject;

	saveObject.AddString("name", name.c_str());
	saveObject.AddInt("Type", type);
	saveObject.AddFloat3("position", float3(position.x, position.y, position.z));
	saveObject.AddColor("diffuse", diffuse);
	saveObject.AddColor("ambient", ambient);
	saveArray.AddObject(saveObject);
}

void Component_Light::Load(JsonObj& loadObject)
{
	name = loadObject.GetString("name", "Light");
	float3 fposition = loadObject.GetFloat3("position");
	position = vec3(fposition.x, fposition.y, fposition.z);
	diffuse = loadObject.GetColor("diffuse");
	ambient = loadObject.GetColor("ambient");
}

void Component_Light::Render()
{
	if (enabled)
	{
		float pos[] = { position.x, position.y, position.z, 1.0f };
		glLightfv(reference, GL_POSITION, pos);
	}
}
