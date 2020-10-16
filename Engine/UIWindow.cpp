#include "UIWindow.h"

UIWindow::UIWindow()
{}

UIWindow::~UIWindow()
{}

void UIWindow::Activate()
{
	is_on = !is_on;
}

bool UIWindow::IsActive()
{
	return is_on;
}