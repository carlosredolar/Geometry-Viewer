#ifndef __FONT_H__
#define __FONT_H__
#include "Component_Graphic.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Libs/ImGui/imgui.h"
#include "GLFont.h"
#include "FTLabel.h"
#include <map>

class JsonArray;
class ResourceTexture;

class Component_Font : public Component_Graphic {

public:
	Component_Font(GameObject* parent);
	~Component_Font();

	void Update() override;
	void OnGUI() override;

	//void Save(JsonArray& saveArray) override;
	//void Load(JsonObj& loadObject) override;

	void InitFonts();

	void Render();

private:

	std::string text;
	//ComponentTransform2D* trans;
	std::map<std::string, std::shared_ptr<GLFont>> fonts;
	std::unique_ptr<FTLabel> label;
	FTLabel::FontFlags aligment;
	ImVec4 color;
	std::string typeFont = "Arial";
	std::string fontSize = "64";
};

#endif //__FONT_H__