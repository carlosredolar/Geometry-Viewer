#ifndef __IMAGE_H__
#define __IMAGE_H__
#include "Component.h"
#include "Libs/MathGeoLib/include/MathGeoLib.h"
#include "Color.h"

class JsonArray;
class ResourceTexture;

class Component_Image : public Component {

public:
	Component_Image();
	~Component_Image();

	void Update() override;
	void OnGUI() override;

	void Save(JsonArray& saveArray) override;
	void Load(JsonObj& loadObject) override;

	void SetResourceUID(uint UID) override;

	ResourceTexture* GetImage();
	void SetImage(ResourceTexture* texture);
private:
	ResourceTexture* image;
	Color color;
};

#endif //__IMAGE_H__
