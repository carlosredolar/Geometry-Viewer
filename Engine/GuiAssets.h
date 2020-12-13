#pragma once
#include "GuiWindow.h"
#include <string>

class ResourceTexture;

struct AssetsIcons
{
	ResourceTexture* folder = nullptr;
	ResourceTexture* model = nullptr;
};

class GuiAssets : public GuiWindow {
public:
	GuiAssets();
	~GuiAssets();
	bool Init() override;
	void Draw() override;

	void RenderDirectory(const char* directory, const char* filterExtension);
	void RenderFolder();
	bool RenderIcon(const char* path, int id, bool isFolder = false);

	const char* GetFileAt(int i);

private:
	std::string currentFolder;
	AssetsIcons icons;
};
