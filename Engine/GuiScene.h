#pragma once

#include "GuiWindow.h"

class GuiScene : public GuiWindow {
public:
	GuiScene();
	~GuiScene();

	void Draw() override;
	void DrawInGameDataOverlay();

};
