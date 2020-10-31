#ifndef __GuiHierarchy_H__
#define __GuiHierarchy_H__

#include "Module.h"
#include "GuiWindow.h"
#include <vector>

class GameObject;

class GuiHierarchy : public GuiWindow
{
public:
	GuiHierarchy();
	~GuiHierarchy();

	bool Start();
	void Draw();
	bool CleanUp();

	void TreeNodeChild(GameObject* gO);

private:
	GameObject* rootNode;
	ImGuiTreeNodeFlags base_flags;
};
#endif __GuiHierarchy_H__