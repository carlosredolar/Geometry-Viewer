#include "Component_Camera.h"

Component_Camera::Component_Camera() : Component(COMPONENT_TYPE::CAMERA, ownerGameObject, enabled)
{
	type = COMPONENT_TYPE::CAMERA;
}


Component_Camera::~Component_Camera()
{
}

void Component_Camera::Enable()
{
	enabled = true;
}

void Component_Camera::Update()
{
}

void Component_Camera::Disable()
{
	enabled = false;
}