#pragma once
#include "GuiWindow.h"

class GameObject;

class GuiHierarchy : public GuiWindow {
public:
	GuiHierarchy();
	~GuiHierarchy();

	void Draw() override;
	void PreorderHierarchy(GameObject* gameObject, int& id);
	void CountID(GameObject* gameObject, int& id);
};
