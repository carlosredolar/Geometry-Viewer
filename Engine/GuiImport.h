#ifndef _WINDOW_IMPORT_H_
#define _WINDOW_IMPORT_H_
#include "GuiWindow.h"
#include "Resource.h"

struct TextureImportingSettings;

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
	ModelImportingSettings modelImportingSettings;
	TextureImportingSettings textureImportingSettings;
	ResourceType currentResourceType;
};

#endif
