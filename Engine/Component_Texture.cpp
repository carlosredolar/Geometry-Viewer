#include "Component_Texture.h"
#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

Component_Texture::Component_Texture(GameObject* ownerGameObject, bool enabled) : Component(COMPONENT_TYPE::MESH, ownerGameObject, enabled)
{

}

Component_Texture::~Component_Texture()
{
	CleanUp();
}

void Component_Texture::Update()
{
	
}

void Component_Texture::Enable()
{
	enabled = true;
}

void Component_Texture::Disable()
{
	enabled = false;
}

bool Component_Texture::IsEnabled()
{
	return enabled;
}

void Component_Texture::CleanUp() 
{
	idTextureImage = -1;
}

uint Component_Texture::GetIdTexture()
{
	return idTextureImage;
}

void Component_Texture::SetIdTexture(uint id)
{
	idTextureImage = id;
}