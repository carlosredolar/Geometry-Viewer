#include "Resource.h"
#include "Application.h"
#include "ModuleJson.h"
#include "glew/include/glew.h"

Resource::Resource(uint uid, ResourceType type)
{
	uid = uid;
	type = type;
}

Resource::~Resource()
{
	assetsFile.clear();
	libraryFile.clear();
}

void Resource::Load(JsonObj& base_object) {}

ResourceType Resource::GetType()
{
	return type;
}

uint Resource::GetUID()
{
	return uid;
}

// ResourceMesh

ResourceMesh::ResourceMesh(uint UID) : Resource(UID, ResourceType::RESOURCE_MESH),
amountVertices(0), vertices(nullptr),
amountIndices(0), indices(nullptr),
amountNormals(0), normals(nullptr),
textureCoords(nullptr), amountTextureCoords(0),
colors(nullptr), buffersCreated(false) {}

ResourceMesh::~ResourceMesh()
{
	if (buffersCreated)
		DeleteBuffers();

	delete vertices;
	vertices = nullptr;
	amountVertices = 0;

	delete indices;
	indices = nullptr;
	amountIndices = 0;

	delete normals;
	normals = nullptr;
	amountNormals = 0;

	delete textureCoords;
	textureCoords = nullptr;
	amountTextureCoords = 0;
}

void ResourceMesh::Load(JsonObj& base_object) {}

void ResourceMesh::GenerateBuffers()
{
	//vertices
	glGenBuffers(1, (GLuint*) & (verticesBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * amountVertices * 3, vertices, GL_STATIC_DRAW);

	//normals
	glGenBuffers(1, (GLuint*) & (normalsBuffer));
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuffer);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * amountVertices * 3, normals, GL_STATIC_DRAW);

	//textures
	glGenBuffers(1, (GLuint*) & (textureCoordsBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * amountVertices * 2, textureCoords, GL_STATIC_DRAW);

	//indices
	glGenBuffers(1, (GLuint*) & (indicesBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * amountIndices, indices, GL_STATIC_DRAW);

	buffersCreated = true;
}

void ResourceMesh::DeleteBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &verticesBuffer);
	verticesBuffer = 0;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indicesBuffer);
	indicesBuffer = 0;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &normalsBuffer);
	normalsBuffer = 0;

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteBuffers(1, &textureCoordsBuffer);
	textureCoordsBuffer = 0;

	buffersCreated = false;
}

// ResourceTexture

ResourceTexture::ResourceTexture(uint UID) : Resource(UID, ResourceType::RESOURCE_TEXTURE),
UUID(0), width(-1), height(-1), data(nullptr), gpuID(0)
{}

ResourceTexture::~ResourceTexture()
{
	data = nullptr;
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourceTexture::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, gpuID);
}

void ResourceTexture::FillData(GLubyte* data, uint UUID, int width, int height)
{
	data = data;
	UUID = UUID;
	width = width;
	height = height;
}

uint ResourceTexture::SaveMeta(JsonObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", uid);
	base_object.AddInt("lastModified", last_modification);
	base_object.AddString("Library path", libraryFile.c_str());

	TextureWrap textureWrap = TextureWrap::REPEAT;
	TextureFiltering textureFiltering = TextureFiltering::NEAREST;

	TextureImportingSettings importingOptions = App->resources->textureImportingSettings;

	base_object.AddInt("texture_wrap", (int)App->resources->textureImportingSettings.textureWrap);
	base_object.AddInt("texture_filtering", (int)App->resources->textureImportingSettings.textureFiltering);
	base_object.AddBool("flip", importingOptions.flip);
	base_object.AddBool("alienify", importingOptions.alienify);
	base_object.AddBool("blur_average", importingOptions.blur_average);
	base_object.AddBool("blur_gaussian", importingOptions.blur_gaussian);
	base_object.AddBool("equalize", importingOptions.equalize);
	base_object.AddBool("negativity", importingOptions.negativity);
	base_object.AddBool("noise", importingOptions.noise);
	base_object.AddFloat("noise_tolerance", importingOptions.noise_tolerance);
	base_object.AddBool("pixelize", importingOptions.pixelize);
	base_object.AddFloat("pixel_size", importingOptions.pixelize_size);
	base_object.AddFloat("gamma_correction", importingOptions.gamma_correction);
	base_object.AddBool("sharpeining", importingOptions.sharpening);
	base_object.AddFloat("sharpening_factor", importingOptions.sharpening_factor);
	base_object.AddInt("sharpening_iterations", importingOptions.sharpening_iterations);
	base_object.AddFloat("contrast", importingOptions.contrast);

	return 1;
}

void ResourceTexture::Load(JsonObj& base_object)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gpuID);
	glBindTexture(GL_TEXTURE_2D, gpuID);

	TextureWrap textureWrap = (TextureWrap)base_object.GetInt("textureWrap", 2);

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

	TextureFiltering textureFiltering = (TextureFiltering)base_object.GetInt("textureFiltering", 0);

	if (textureFiltering == TextureFiltering::NEAREST) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

}

uint ResourceTexture::GetID() 
{
	return UUID; 
}

int ResourceTexture::GetWidth() 
{ 
	return width; 
}

int ResourceTexture::GetHeight() 
{ 
	return height; 
}

GLubyte* ResourceTexture::GetData() 
{ 
	return data; 
}

uint ResourceTexture::GetGpuID() 
{ 
	return gpuID; 
}

std::string ResourceTexture::GetPath()
{
	return path;
}

// ResourceModel

ResourceModel::ResourceModel(uint UID) : Resource(UID, ResourceType::RESOURCE_MODEL) {}

ResourceModel::~ResourceModel()
{
	nodes.clear();
	meshes.clear();
	materials.clear();
	textures.clear();

	for (size_t i = 0; i < lights.size(); i++)
	{
		delete lights[i];
		lights[i] = nullptr;
	}
	lights.clear();

	//for (size_t i = 0; i < cameras.size(); i++)
	//{
	//	delete cameras[i];
	//	cameras[i] = nullptr;
	//}
	//cameras.clear();
}

uint ResourceModel::Save(JsonObj& base_object)
{
	return 1;
}

uint ResourceModel::SaveMeta(JsonObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", uid);
	base_object.AddString("Library path", App->resources->GetLibraryPath(uid));
	base_object.AddInt("lastModified", last_modification);

	ModelImportingSettings importingOptions = App->resources->modelImportingSettings;

	base_object.AddFloat("global scale", (float)importingOptions.globalScale);
	base_object.AddInt("forward axis", (int)importingOptions.forwardAxis);
	base_object.AddInt("up axis", (int)importingOptions.upAxis);
	base_object.AddBool("ignore cameras", importingOptions.ignoreCameras);
	base_object.AddBool("ignore lights", importingOptions.ignoreLights);

	JsonArray nodes_array = base_object.AddArray("Nodes");

	for (size_t i = 0; i < nodes.size(); i++)
	{
		JsonObj nodeObject;

		nodeObject.AddString("Name", nodes[i].name.c_str());
		nodeObject.AddInt("UID", nodes[i].UID);
		nodeObject.AddInt("Parent UID", nodes[i].parentUID);

		nodeObject.AddFloat3("Position", nodes[i].position);
		nodeObject.AddQuaternion("Rotation", nodes[i].rotation);
		nodeObject.AddFloat3("Scale", nodes[i].scale);

		if (nodes[i].meshID != -1)
		{
			nodeObject.AddInt("MeshID", nodes[i].meshID);
			nodeObject.AddString("mesh_library_path", App->resources->GetLibraryPath(nodes[i].meshID));
		}

		if (nodes[i].materialID != -1)
		{
			nodeObject.AddInt("MaterialID", nodes[i].materialID);
			nodeObject.AddString("material_library_path", App->resources->GetLibraryPath(nodes[i].materialID));
		}

		nodes_array.AddObject(nodeObject);
	}

	JsonArray lights_array = base_object.AddArray("Lights");
	for (size_t i = 0; i < lights.size(); i++)
	{
		lights[i]->Save(lights_array);
	}

	//JsonArray cameras_array = base_object.AddArray("Cameras");
	//for (size_t i = 0; i < cameras.size(); i++)
	//{
	//	cameras[i]->Save(cameras_array);
	//}

	return 1;
}

// ResourceMaterial

ResourceMaterial::ResourceMaterial(uint UID) : Resource(UID, ResourceType::RESOURCE_MATERIAL), diffuseTextureUID(0) {}

ResourceMaterial::~ResourceMaterial() {}