#include "Component_Canvas.h"

Component_Canvas::Component_Canvas(GameObject* parent) : Component(CANVAS, parent)
{
}

Component_Canvas::~Component_Canvas()
{
}

void Component_Canvas::Update() 
{
}

void Component_Canvas::OnGUI() 
{
}

void Component_Canvas::Save(JsonArray& saveArray) 
{
}

void Component_Canvas::Load(JsonObj& loadObject) 
{
}

void Component_Canvas::Resize(int width, int height)
{
}