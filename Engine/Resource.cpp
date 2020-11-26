#include "Resource.h"
#include "ModuleJson.h"

Resource::Resource(uint uid, ResourceType type)
{
	_uid = uid;
	_type = type;
}

Resource::~Resource()
{
	assetsFile.clear();
	libraryFile.clear();
}

ResourceType Resource::GetType()
{
	return _type;
}

uint Resource::GetUID()
{
	return _uid;
}

//ResourceMesh

ResourceMesh::ResourceMesh(uint UID) : Resource(UID, ResourceType::RESOURCE_MESH),
amountVertices(-1), vertices(nullptr),
amountIndices(-1), indices(nullptr),
amountNormals(-1), normals(nullptr),
textureCoords(nullptr), colors(nullptr) {}

ResourceMesh::~ResourceMesh() {}

void ResourceMesh::Load(JsonObj& base_object)
{

}

//ResourceTexture

ResourceTexture::ResourceTexture(uint UID) : Resource(UID, ResourceType::RESOURCE_TEXTURE), id(0), width(-1), height(-1), data(nullptr) {}

ResourceTexture::~ResourceTexture() {}

uint ResourceTexture::SaveMeta(JsonObj& baseObject, uint last_modification)
{
	baseObject.AddInt("UID", _uid);
	baseObject.AddInt("lastModified", last_modification);
	//base_object.AddInt("width", width);
	//base_object.AddInt("height", height);

	return 1;
}