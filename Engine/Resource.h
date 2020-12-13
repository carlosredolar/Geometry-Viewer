#pragma once
#include "Globals.h"

#include <string>

class JsonObj;

enum ResourceType 
{
	RESOURCE_MODEL,
	RESOURCE_MESH,
	RESOURCE_MATERIAL,
	RESOURCE_TEXTURE,
	RESOURCE_SCENE,
	RESOURCE_UNKNOWN
};

class Resource 
{
public:
	Resource(uint uid, ResourceType type);
	virtual ~Resource();

	ResourceType GetType();
	uint GetUID();

	virtual uint Save(JsonObj& savingObj) { return -1; };
	virtual uint SaveMeta(JsonObj& savingObj, uint lastMod) { return -1; };
	virtual void Load(JsonObj& loadingObj) {};

public: 
	std::string name;
	std::string assetsFile;
	std::string libraryFile;

	int instances = 0;

protected:
	uint _uid = 0;
	ResourceType _type = RESOURCE_UNKNOWN;
};
