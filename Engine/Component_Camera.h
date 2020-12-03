#pragma once
#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Component.h"

class Component_Camera : public Component
{
public:
	Component_Camera();
	~Component_Camera();

	void Enable();
	void Update();
	void Disable();

};

#endif //__COMPONENTCAMERA_H__