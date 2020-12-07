#include "Component.h"

Component::Component(ComponentType type, GameObject* ownerGameObject, bool enabled) : type(type), ownerGameObject(ownerGameObject), enabled(enabled) {}

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
}

void Component::Disable()
{
	enabled = false;
}

bool Component::IsEnabled()
{
	return enabled;
}

ComponentType Component::GetType()
{
	return type;
}
