#include "Component.h"

Component::Component(ComponentType type) : type(type), enabled(true), ownerGameObject(nullptr) {}

Component::Component(ComponentType _type, GameObject* gameObject)
{
	type = _type;
    ownerGameObject = gameObject;
}

Component::~Component()
{
    ownerGameObject = nullptr;
}

void Component::Update(){}

void Component::Enable() { enabled = true; }

void Component::Disable() { enabled = false; }

bool Component::IsEnabled()
{
    return enabled;
}

ComponentType Component::GetType()
{
    return type;
}

void Component::SetGameObject(GameObject* g_gameObject)
{
    ownerGameObject = g_gameObject;
}

GameObject* Component::GetGameObject()
{
    return ownerGameObject;
}

void Component::SetResourceUID(uint UID)
{
    _resourceUID = UID;
}
