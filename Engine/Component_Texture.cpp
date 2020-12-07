#include "Component_Texture.h"

#include "ModuleImport.h"

Component_Texture::Component_Texture(GameObject* ownerGameObject, bool enabled) : Component(COMPONENT_TYPE::MESH, ownerGameObject, enabled)
{
	type = COMPONENT_TYPE::TEXTURE;
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
	delete texture;
	sizeTexture.clear();
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

	sizeTexture = std::to_string(texture->w);
	sizeTexture += "x";
	sizeTexture += std::to_string(texture->h).c_str();
}

const char* Component_Texture::GetName()
{
	return texture->name.c_str();
}

const char* Component_Texture::GetPath()
{
	return texture->path.c_str();
}

const char* Component_Texture::GetSize()
{
	return sizeTexture.c_str();
}