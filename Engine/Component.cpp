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

GameObject* Component::GetGameObject()
{
    return ownerGameObject;
}

void Component::SetGameObject(GameObject* gameObject)
{
    ownerGameObject = gameObject;
}

ComponentType Component::GetType()
{
    return type;
}

Resource* Component::GetResource(ResourceType type)
{
    return nullptr;
}

void Component::SetResourceUID(uint UID)
{
    resourceUID = UID;
}