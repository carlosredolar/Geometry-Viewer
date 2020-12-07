#include "Globals.h"
#include "Light.h"
#include <gl/GL.h>

#include "ModuleJson.h"

Light::Light() : Component(), ref(-1), position(0.0f, 0.0f, 0.0f)
{
	type = ComponentType::LIGHT;
}

Light::Light(GameObject* gameObject) : Component(LIGHT, gameObject), ref(-1), position(0.0f, 0.0f, 0.0f)
{
	type = ComponentType::LIGHT;
}

Light::~Light()
{
}

void Light::Init()
{
	glLightfv(ref, GL_AMBIENT, &ambient);
	glLightfv(ref, GL_DIFFUSE, &diffuse);
}

void Light::Update()
{
	Render();
}

void Light::SetPos(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Light::Render()
{
	if (enabled)
	{
		float pos[] = { position.x, position.y, position.z, 1.0f };
		glLightfv(ref, GL_POSITION, pos);
	}
}

void Light::OnGUI()
{

}

void Light::Save(JsonArray& save_array)
{
	JsonObj save_object;

	save_object.AddFloat3("position", float3(position.x, position.y, position.z));
	save_object.AddColor("diffuse", diffuse);
	save_object.AddColor("ambient", ambient);

	save_array.AddObject(save_object);
}

void Light::Load(JsonObj& load_object)
{
	float3 fposition = load_object.GetFloat3("position");
	position = vec3(fposition.x, fposition.y, fposition.z);
	diffuse = load_object.GetColor("diffuse");
	ambient = load_object.GetColor("ambient");
}

void Light::Active(bool active)
{
	enabled = active;

	if (enabled)
		glEnable(ref);
	else
		glDisable(ref);
}