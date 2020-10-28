#include "Component.h"

Component::Component(COMPONENT_TYPE type, GameObject* ownerGameObject, bool enabled) : type(type), ownerGameObject(ownerGameObject), enabled(enabled) {}

Component::~Component()
{
	ownerGameObject = nullptr;
}

void Component::Update()
{
	return;
}

void Component::Enable()
{
	enabled = true;
	return;
}

void Component::Disable()
{
	enabled = false;
	return;
}

bool Component::IsEnabled()
{
	return enabled;
}

Component::COMPONENT_TYPE Component::GetType()
{
	return COMPONENT_TYPE();
}
