#include "Application.h"
#include "GuiHierarchy.h"
#include "ModuleGui.h"
#include "GuiInspector.h"
#include "ModuleScene.h"

using namespace ImGui;

GuiHierarchy::GuiHierarchy() : GuiWindow()
{
	is_on = true;
	base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
}

GuiHierarchy::~GuiHierarchy()
{}

bool GuiHierarchy::Start()
{
	bool ret = true;
	rootNode = App->scene->GetGameObject(0);
	return ret;
}

bool GuiHierarchy::CleanUp()
{
	bool ret = true;

	return ret;
}

void GuiHierarchy::Draw()
{
	Begin("Hierarchy", &is_on);

	if (TreeNode(rootNode->GetName()))
	{
		
		TreeNodeChild(rootNode);

		ImGui::TreePop();
	}

	ImGui::End();
}

void GuiHierarchy::TreeNodeChild(GameObject* gO)
{
	if (gO->GetChilds()->size() > 0)
	{
		static int selection_mask = (1 << 2);
		int node_clicked = -1;
		for (int j = 0; j < gO->GetChilds()->size(); j++)
		{
			GameObject* gOChild = gO->GetChilds()->at(j);
			ImGuiTreeNodeFlags node_flags = base_flags; // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
			const bool is_selected = (selection_mask & (1 << gOChild->id)) != 0;

			if (is_selected && gOChild->selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			if (gOChild->GetChilds()->size() > 0)
			{
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)j, node_flags, gOChild->GetName(), j);
				if (ImGui::IsItemClicked())
					node_clicked = gOChild->id;
				if (node_open)
				{
					TreeNodeChild(gOChild);
					ImGui::TreePop();
				}
			}
			else
			{
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)j, node_flags, gOChild->GetName(), j);
				if (ImGui::IsItemClicked())
					node_clicked = gOChild->id;
			}

		}
		if (node_clicked != -1)
		{
			GameObject* selectedGO = App->scene->GetGameObject(node_clicked);
			//std::string logMessage = selectedGO->GetName();
			//logMessage += " selected";
			//LOG(logMessage.c_str());

			App->scene->SelectGameObject(selectedGO);

			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
	}
}