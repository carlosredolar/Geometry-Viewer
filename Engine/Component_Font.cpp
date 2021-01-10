#pragma once
#include "Component_Font.h"
#include "Application.h"
#include "Globals.h"
#include "Component_CanvasRenderer.h"
#include "Component_Transform.h"
#include "GameObject.h"
#include "ModuleJson.h"
#include "ResourceTexture.h"
#include "GuiAssets.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Glew/include/glew.h"

//#include "ModuleSceneIntro.h"
//#include <iostream>


Component_Font::Component_Font(GameObject* parent) : Component_Graphic(ComponentType::FONT, parent)
{
	InitFonts();
}

Component_Font::~Component_Font()
{
}

void Component_Font::Update()
{
	Render();
}

void Component_Font::InitFonts()
{
	fonts["Arial"] = std::shared_ptr<GLFont>(new GLFont("Assets/EngineAssets/Fonts/arial.ttf"));
	fonts["Calibri"] = std::shared_ptr<GLFont>(new GLFont("Assets/EngineAssets/Fonts/Calibri Regular.ttf"));
	fonts["Roboto"] = std::shared_ptr<GLFont>(new GLFont("Assets/EngineAssets/Fonts/Roboto-Regular.ttf"));
	fonts["EastSeaDokdo"] = std::shared_ptr<GLFont>(new GLFont("Assets/EngineAssets/Fonts/EastSeaDokdo-Regular.ttf"));

	text = "HOLA MUNDO";

	label = std::unique_ptr<FTLabel>(new FTLabel(
		fonts["Arial"],
		text.c_str(),
		0,
		0,
		App->window->width,
		App->window->height
	));
	label->setColor(0.89, 0.26, 0.3, 0.9);
	label->setPixelSize(64);
	label->setAlignment(FTLabel::FontFlags::RightAligned);
	label->appendFontFlags(FTLabel::FontFlags::Indented);

}

void Component_Font::Render()
{
	label.get()->setFont(fonts[typeFont]);
	if (fontSize == "") fontSize = "0";
	label.get()->setPixelSize(std::stoi(fontSize));
	label.get()->setText((char*)text.c_str());
	label.get()->setAlignment(aligment);
	label.get()->setPosition(0, 0);
	label.get()->setColor(color.x, color.y, color.z, color.w);

	label.get()->render();
}

void Component_Font::OnGUI()
{
}