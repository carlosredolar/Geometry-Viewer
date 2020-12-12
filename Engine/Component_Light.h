
#pragma once
#include "Color.h"
#include "glmath.h"
#include "Component.h"

class JsonArray;
class JsonObj;
class GameObject;

class Component_Light : public Component {
public:
	Component_Light();
	Component_Light(GameObject* gameObject);
	~Component_Light();

	void Init();
	void Update() override;
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();
	void OnGUI() override;
	void Save(JsonArray& save_array);
	void Load(JsonObj& load_object);

public:
	vec3 position;
	Color ambient;
	Color diffuse;

	int ref;
};