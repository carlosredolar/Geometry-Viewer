
#pragma once
#include "Color.h"
#include "glmath.h"
#include "Component.h"

class GameObject;
class JsonArray;
class JsonObj;


class Component_Light : public Component {
public:
	Component_Light();
	Component_Light(GameObject* gameObject);
	~Component_Light();

	void Init();
	void Update() override;
	void SetPosition(float x, float y, float z);
	void Enable(bool enable);
	
	void OnGUI() override;

	void Save(JsonArray& saveArray);
	void Load(JsonObj& loadObject);

	void Render();

public:
	vec3 position;
	Color ambient;
	Color diffuse;

	int reference;
};