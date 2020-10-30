#ifndef __GuiInspector_H__
#define __GuiInspector_H__

#include "Module.h"
#include "GuiWindow.h"
#include "Component_Transform.h"
#include "Component_Mesh.h"
#include "Component_Texture.h"

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
	Component_Transform* transformGO;
	Component_Mesh* meshGO;
	Component_Texture* textureGO;
	char buf[128];
};

#endif // !__GuiInspector_H__