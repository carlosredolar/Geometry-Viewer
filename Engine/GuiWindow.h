#ifndef __GUIWINDOW_H__
#define __GUIWINDOW_H__

#include "Application.h"
#include "Globals.h"
#include "SDL/include/SDL.h"

class GuiWindow
{
public:
	GuiWindow();
	virtual ~GuiWindow();

	virtual bool IsActive();
	virtual void Draw() = 0;


public:
	bool is_on = false;
};

#endif 