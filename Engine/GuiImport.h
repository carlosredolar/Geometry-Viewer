#ifndef _WINDOW_IMPORT_H_
#define _WINDOW_IMPORT_H_
#include "GuiWindow.h"
#include "ImportingSettings.h"
#include "Resource.h"

class GuiImport : public GuiWindow {
public:
	GuiImport();
	~GuiImport();

	void Draw() override;
	bool DrawModelImportingWindow();
	bool DrawTextureImportingWindow();

	void Enable(const char* file, ResourceType resourceType);

private: 
	const char* fileToImport;
	ModelImportingSettings modelImportSettings;
	TextureImportingSettings textureImportSettings;
	ResourceType currentResourceType;
};

#endif
