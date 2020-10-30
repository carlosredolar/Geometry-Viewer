#ifndef __GUICONSOLE_H__
#define __GUICONSOLE_H__

#include "Module.h"
#include "GuiWindow.h"

class GuiConsole : public GuiWindow
{
public:
	GuiConsole();
	~GuiConsole();

	bool Start();
	void Draw();
	bool CleanUp();
};

#endif // !__GUICONSOLE_H__