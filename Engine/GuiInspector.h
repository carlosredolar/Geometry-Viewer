#ifndef __GuiInspector_H__
#define __GuiInspector_H__

#include "Module.h"
#include "GuiWindow.h"
#include "Component_Transform.h"

class GameObject;

class GuiInspector : public GuiWindow
{
public:
	GuiInspector();
	~GuiInspector();

	bool Start();
	void Draw();
	bool CleanUp();

	void Select(GameObject* selected);

private:
	GameObject* gameObjectSelected;
	Component_Transform* goTransform;
	char buf[128];
};

#endif // !__GuiInspector_H__