#include "Globals.h"
#include "Application.h"
#include "ModuleImport.h"
#include "GameObject.h"
#include "Component.h"
#include "Component_Mesh.h"
#include "Component_Transform.h"
#include "Component_Texture.h"
#include "Defs.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil/include/ilut.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#pragma comment (lib, "glu32.lib")   
#pragma comment (lib, "opengl32.lib") 
#pragma comment (lib, "Glew/libx86/glew32.lib")

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}


ModuleImport::~ModuleImport()
{

}

bool ModuleImport::Init() {

	bool ret = true;
	//Log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	//Initialize DevIL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	//Check for any error
	ILenum errorIL = ilGetError();
	if (errorIL != IL_NO_ERROR)
	{
		printf("Error initializing DevIL! %s\n", iluErrorString(errorIL));
		ret = false;
	}

	return ret;
}

update_status ModuleImport::Update(float dt) 
{

	return UPDATE_CONTINUE;
}


bool ModuleImport::CleanUp() {

	//Detach the log stream
	aiDetachAllLogStreams();

	//Delete textures
	std::vector<textureInfo*>::iterator currentT = textures.begin();
	for (; currentT != textures.end(); currentT++) {
		iluDeleteImage((*currentT)->id);
	}
	textures.clear();

	//Delete meshes
	meshes.clear();

	return true;
}

void ModuleImport::LoadMesh(const char* filepath, const char* originalPath)
{
	char* buffer = nullptr;

	uint bytesFile = App->fm->Load(filepath, &buffer);

	const aiScene* scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	RELEASE_ARRAY(buffer);

	if (scene != nullptr && scene->HasMeshes()) 
	{
		std::vector<Component_Mesh> loadedMeshes;
		bool ret = true;

		std::string filename;
		App->fm->SplitFilePath(filepath, nullptr, &filename, nullptr);

		GameObject* sceneGameObject = App->scene->CreateGameObject(filename.c_str(), nullptr, true);

		//Loading and creating transform component
		aiVector3D translation, scaling;
		aiQuaternion rotation;

		scene->mRootNode->mTransformation.Decompose(scaling, rotation, translation);
		float3 pos(translation.x, translation.y, translation.z);
		Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
		float3 scale(scaling.x, scaling.y, scaling.z);

		sceneGameObject->GetComponent<Component_Transform>()->SetTransform(pos, rot, scale);

		ret = LoadSceneMeshes(scene, scene->mRootNode, sceneGameObject, originalPath);

		if (ret && loadedMeshes.size() > 0)LOG("Loaded %i mesh(es)!", loadedMeshes.size())
		aiReleaseImport(scene);
	}
}

bool ModuleImport::LoadSceneMeshes(const aiScene* scene, const aiNode* parent, GameObject* gOParent, const char* originalPath)
{
	bool ret = false;
	for (int i = 0; i < parent->mNumChildren; i++) ret = LoadNodeMeshes(scene, parent->mChildren[i], gOParent, originalPath);

	return ret;
}

bool ModuleImport::LoadNodeMeshes(const aiScene* scene, const aiNode* node, GameObject* parent, const char* originalPath)
{
	GameObject* newGameObject = App->scene->CreateGameObject(node->mName.C_Str(), parent, true);
	aiString texture_path_to_read;

	//Loading and creating transform component
	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);
	float3 pos(translation.x, translation.y, translation.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	float3 scale(scaling.x, scaling.y, scaling.z);

	newGameObject->GetComponent<Component_Transform>()->SetTransform(pos, rot, scale);
	App->scene->SelectGameObject(newGameObject);

	//Loading mesh
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* nodeMesh = scene->mMeshes[node->mMeshes[i]];

		std::vector<float3> vertex;
		std::vector<float3> normals;
		std::vector<float2> textureCoords;
		std::vector<uint> index;

		vertex.reserve(nodeMesh->mNumVertices); //Reserve vertex space
		if (nodeMesh->HasNormals()) normals.reserve(nodeMesh->mNumVertices); //Reserve normal space
		textureCoords.reserve(nodeMesh->mNumVertices); //Reserve texture space
		index.reserve(nodeMesh->mNumFaces * 3); //Reserve index space
		
		//Loading all
		LoadVertexNormalsTexturesIndex(nodeMesh, vertex, normals, textureCoords, index);
		if (vertex.size() == 0) { LOG("Error loading vertices in this mesh") return false; }
		else LOG("New mesh with %i vertices", vertex.size());
		if (index.size() == 0) { LOG("Error loading indices in this mesh") return false; }
		else LOG("And with %i indices", index.size());

		//Create new component mesh
		meshInfo* newMesh = new meshInfo;
		newMesh->name = newGameObject->GetName();
		meshes.push_back(newMesh);
		Component_Mesh* newMeshComponent = (Component_Mesh*)newGameObject->CreateComponent(Component::COMPONENT_TYPE::MESH);
		newMeshComponent->GenerateMesh(newMesh, vertex, index, normals, textureCoords);

		//Textures
		aiMaterial* Object_material = scene->mMaterials[nodeMesh->mMaterialIndex];
		//aiGetMaterialTexture(Object_material, aiTextureType_DIFFUSE, nodeMesh->mMaterialIndex, &texture_path_to_read);
		Object_material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path_to_read);

		std::string file_path(texture_path_to_read.C_Str());
		std::string fileName;
		std::string fileExtension;
		App->fm->SplitFilePath(file_path.c_str(), nullptr, &fileName, &fileExtension);
		if (originalPath != nullptr)
		{
			file_path = App->fm->NormalizePath(originalPath);
			file_path.erase(file_path.find_last_of("/") + 1);
			file_path = file_path + fileName + "." + fileExtension;
		}
		else 
			file_path = App->fm->GetInternalFolder(fileExtension.c_str()) + "/" + fileName + "." + fileExtension;

		if (texture_path_to_read.length > 0) {
			ImportExternalFiles(file_path.c_str());
		}
	}

	LoadSceneMeshes(scene, node, newGameObject, originalPath);
	return true;
}

update_status ModuleImport::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleImport::LoadVertexNormalsTexturesIndex(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords, std::vector<uint>& indices)
{
	bool ret = true;
	for (uint i = 0; i < mesh->mNumVertices; i++) {
		// Vertex
		float3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertices.push_back(vector);

		// Normals
		if (mesh->HasNormals()){
			float3 newNormal;
			newNormal.x = mesh->mNormals[i].x;
			newNormal.y = mesh->mNormals[i].y;
			newNormal.z = mesh->mNormals[i].z;
			normals.push_back(newNormal);
		}

		// Texture coordinates
		float2 newTextCord;
		if (mesh->mTextureCoords[0])
		{
			newTextCord.x = mesh->mTextureCoords[0][i].x;
			newTextCord.y = mesh->mTextureCoords[0][i].y;
		}
		else newTextCord = float2(0.0f, 0.0f);
		textureCoords.push_back(newTextCord);
	}
	//Load indices
	for (uint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3) {
			LOG("Error! Not all faces are made out of triangles!")
			ret = false;
			break;
		}
		for (uint j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
	}

	return ret;
}

textureInfo* ModuleImport::LoadTexture(const char* path)
{
	textureInfo* newTexture = new textureInfo;

	std::string textureName;
	std::string extension;
	App->fm->SplitFilePath(path, nullptr, &textureName, &extension);
	newTexture->name = textureName + "." + extension;

	newTexture->path = path;

	char* buffer = nullptr;
	uint bytesFile = App->fm->Load(path, &buffer);
	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, bytesFile);
	newTexture->id = ilutGLBindTexImage();

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &newTexture->w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &newTexture->h);
	
	RELEASE_ARRAY(buffer);

	textures.push_back(newTexture);

	return newTexture;
}

uint ModuleImport::LoadDefaultTexture()
{
	uint textID = 1;

	GLubyte checkerImage[20][20][4];

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textID);
	glBindTexture(GL_TEXTURE_2D, textID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 20, 20, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	return textID;
}

void ModuleImport::ImportExternalFiles(const char* path)
{
	std::string normalizedPath = App->fm->NormalizePath(path);

	std::string finalPath;

	std::string fileName;
	std::string fileExtension;
	App->fm->SplitFilePath(path, nullptr, &fileName, &fileExtension);
	fileName += ".";
	fileName += fileExtension;
	if (!App->fm->ExistsFile(fileName.c_str(), fileExtension.c_str()))
	{
		if (App->fm->ImportFile(normalizedPath.c_str(), finalPath))
		{
			ExtensionClassifier(finalPath.c_str(), path);
		}
	}
	else
	{
		std::string internalPath = App->fm->GetInternalFolder(fileExtension.c_str());
		internalPath += "/";
		internalPath += fileName;
		ExtensionClassifier(internalPath.c_str());
	}
}

void ModuleImport::ExtensionClassifier(const char* path, const char* originalPath)
{
	std::string extension;
	App->fm->SplitFilePath(path, nullptr, nullptr, &extension);
	if (strcmp(extension.c_str(), "fbx") == 0 || strcmp(extension.c_str(), "FBX") == 0)
	{
		LoadMesh(path, originalPath);
	}
	else if (strcmp(extension.c_str(), "png") == 0 || strcmp(extension.c_str(), "jpg") == 0 || strcmp(extension.c_str(), "tga") == 0)
	{
		if (App->scene->GetSelectedGameObject() != nullptr)
		{
			if (App->scene->GetSelectedGameObject()->GetComponent<Component_Texture>() != nullptr)
			{
				App->scene->GetSelectedGameObject()->GetComponent<Component_Texture>()->SetTexture(LoadTexture(path));
			}
			else if (App->scene->GetSelectedGameObject()->GetComponent<Component_Mesh>() != nullptr)
			{
				((Component_Texture*)App->scene->GetSelectedGameObject()->CreateComponent(Component::COMPONENT_TYPE::TEXTURE))->SetTexture(LoadTexture(path));
				App->gui->SelectGameObject(App->scene->GetSelectedGameObject());
			}
			else LOG("This GameOject doesn't have any mesh or texture components so the texture will not be applyed to anything.");
		}
		else LOG("There is no GameObject selected. Please select a GameObject and drop the texture again.");
	}
}