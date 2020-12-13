#include "ResourceTexture.h"
#include "ModuleJson.h"
#include "Libs/Glew/include/glew.h"
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
	

	TextureImportSettings importOptions = App->resources->textureImportSettings;

	savingObj.AddInt("texture_wrap", (int)App->resources->textureImportSettings.textureWrap);
	savingObj.AddInt("texture_filtering", (int)App->resources->textureImportSettings.textureFiltering);
	savingObj.AddBool("flip", importOptions.flip);
	savingObj.AddBool("blur_average", importOptions.blurAverage);
	savingObj.AddInt("sharpening_iterations", importOptions.sharpeningIterations);
	savingObj.AddBool("blur_gaussian", importOptions.blurGaussian);
	savingObj.AddFloat("pixel_size", importOptions.pixelizeSize);
	savingObj.AddFloat("gamma_correction", importOptions.gammaCorrection);
	savingObj.AddFloat("sharpening_factor", importOptions.sharpeningFactor);
	savingObj.AddBool("equalize", importOptions.equalize);
	savingObj.AddBool("negativity", importOptions.negativity);
	savingObj.AddBool("noise", importOptions.noise);
	savingObj.AddBool("alienify", importOptions.alienify);
	savingObj.AddFloat("noise_tolerance", importOptions.noiseTolerance);
	savingObj.AddBool("pixelize", importOptions.pixelize);
	savingObj.AddBool("sharpeining", importOptions.sharpening);
	savingObj.AddFloat("contrast", importOptions.contrast);

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

