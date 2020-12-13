#pragma once
#include "Globals.h"
#include "Resource.h"

typedef unsigned char GLubyte;
class JsonObj;

class ResourceTexture : public Resource {
public:
	ResourceTexture(uint UID);
	~ResourceTexture();

	uint SaveMeta(JsonObj& savingObj, uint lastMod) override;
	void Load(JsonObj& loadingObj) override;

	void ApplyData(GLubyte* data, uint id, int width, int height);

	void GenerateBuffers();
	void BindTexture();

	uint GetID();

	int GetWidth();
	int GetHeight();

	GLubyte* GetData();
	uint GetGpuID();

private:
	uint textureID;

	int textureWidth;
	int textureHeight;

	GLubyte* textureData;
	uint gpuID;
};
