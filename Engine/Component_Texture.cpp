#include "Component_Texture.h"

#include "ModuleImport.h"

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
	
}

uint Component_Texture::GetIdTexture()
{
	return texture->id;
}

void Component_Texture::SetIdTexture(uint id)
{
	texture->id = id;
}

void Component_Texture::SetTexture(textureInfo* tex)
{
	texture = tex;
}