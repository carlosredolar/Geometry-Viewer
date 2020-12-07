#include "Component_Material.h"

#include "ModuleImport.h"

Component_Material::Component_Material(GameObject* ownerGameObject, bool enabled) : Component(ComponentType::MESH, ownerGameObject, enabled)
{

}

Component_Material::~Component_Material()
{
	CleanUp();
}

void Component_Material::Update()
{
	
}

void Component_Material::CleanUp() 
{
	delete texture;
	sizeTexture.clear();
}

uint Component_Material::GetIdTexture()
{
	return texture->UUID;
}

void Component_Material::SetIdTexture(uint UUID)
{
	texture->UUID = UUID;
}

void Component_Material::SetTexture(textureInfo* tex)
{
	texture = tex;

	sizeTexture = std::to_string(texture->w);
	sizeTexture += "x";
	sizeTexture += std::to_string(texture->h).c_str();
}

const char* Component_Material::GetName()
{
	return texture->name.c_str();
}

const char* Component_Material::GetPath()
{
	return texture->path.c_str();
}

const char* Component_Material::GetSize()
{
	return sizeTexture.c_str();
}