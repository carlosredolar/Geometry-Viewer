#ifndef __IMAGE_H__
#define __IMAGE_H__
#include "Component_Graphic.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"

class JsonArray;
class ResourceTexture;

class Component_Image : public Component_Graphic {

public:
	Component_Image(GameObject* parent);
	~Component_Image();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void SetResourceUID(uint UID) override;

	void CheckersTexDefault();

	ResourceTexture* GetImage();
	void SetImage(ResourceTexture* texture);
private:
	bool checkersImageActive;

	uint checkersID;

	ResourceTexture* image;
	Color color;
	float2 texTile = float2::one;
};

#endif //__IMAGE_H__
