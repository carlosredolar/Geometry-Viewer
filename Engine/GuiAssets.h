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
	const char* GetFileAt(int i);

private:
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);
	void DrawCurrentFolder();
	bool DrawIcon(const char* path, int id, bool isFolder = false);

private:
	std::string current_folder;
	AssetsIcons icons;
};
