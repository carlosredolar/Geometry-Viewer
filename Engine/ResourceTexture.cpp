#include "ResourceTexture.h"
#include "ModuleJson.h"
#include "glew/include/glew.h"
#include "Application.h"

ResourceTexture::ResourceTexture(uint UID) : Resource(UID, ResourceType::RESOURCE_TEXTURE),
textureID(0), textureWidth(-1), textureHeight(-1), textureData(nullptr) , gpuID(0)
{}

ResourceTexture::~ResourceTexture() 
{
	textureData = nullptr;
	glDeleteTextures(1, &gpuID);
}

void ResourceTexture::GenerateBuffers()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gpuID);
	glBindTexture(GL_TEXTURE_2D, gpuID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourceTexture::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, gpuID);
}

uint ResourceTexture::SaveMeta(JsonObj& savingObj, uint lastMod)
{
	savingObj.AddInt("UID", _uid);
	savingObj.AddInt("lastModified", lastMod);
	savingObj.AddString("Library path", libraryFile.c_str());
	TextureWrap textureWrap = TextureWrap::REPEAT;
	TextureFiltering textureFiltering = TextureFiltering::NEAREST;
	
	TextureImportingSettings importingSettings = App->resources->textureImportSettings;

	savingObj.AddInt("texture_wrap", (int)App->resources->textureImportSettings.textureWrap);
	savingObj.AddInt("texture_filtering", (int)App->resources->textureImportSettings.textureFiltering);
	savingObj.AddBool("flip", importingSettings.flip);
	savingObj.AddBool("blur_average", importingSettings.blurAverage);
	savingObj.AddInt("sharpening_iterations", importingSettings.sharpeningIterations);
	savingObj.AddBool("blur_gaussian", importingSettings.blurGaussian);
	savingObj.AddFloat("pixel_size", importingSettings.pixelizeSize);
	savingObj.AddFloat("gamma_correction", importingSettings.gammaCorrection);
	savingObj.AddFloat("sharpening_factor", importingSettings.sharpeningFactor);
	savingObj.AddBool("equalize", importingSettings.equalize);
	savingObj.AddBool("negativity", importingSettings.negativity);
	savingObj.AddBool("noise", importingSettings.noise);
	savingObj.AddBool("alienify", importingSettings.alienify);
	savingObj.AddFloat("noise_tolerance", importingSettings.noiseTolerance);
	savingObj.AddBool("pixelize", importingSettings.pixelize);
	savingObj.AddBool("sharpeining", importingSettings.sharpening);
	savingObj.AddFloat("contrast", importingSettings.contrast);

	return 1;
}

void ResourceTexture::Load(JsonObj& loadingObj)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gpuID);
	glBindTexture(GL_TEXTURE_2D, gpuID);

	TextureWrap textureWrap = (TextureWrap)loadingObj.GetInt("textureWrap", 2);

	switch (textureWrap)
	{
	case TextureWrap::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	case TextureWrap::CLAMP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case TextureWrap::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TextureWrap::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	default:
		break;
	}

	TextureFiltering textureFiltering = (TextureFiltering)loadingObj.GetInt("textureFiltering", 0);

	if (textureFiltering == TextureFiltering::LINEAR) 
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else 
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void ResourceTexture::ApplyData(GLubyte* data, uint id, int width, int height)
{
	textureData = data;
	textureID = id;
	textureWidth = width;
	textureHeight = height;
}

uint ResourceTexture::GetID() 
{	
	return textureID; 
}

int ResourceTexture::GetWidth() 
{ 
	return textureWidth; 
}

int ResourceTexture::GetHeight() 
{	
	return textureHeight;
}

GLubyte* ResourceTexture::GetData() 
{ 
	return textureData; 
}

uint ResourceTexture::GetGpuID() 
{ 
	return gpuID; 
}

