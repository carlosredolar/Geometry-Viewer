#include "Globals.h"
#include "Application.h"
#include "ModuleImport.h"
#include "Defs.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
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

	return true;
}

void ModuleImport::LoadMesh(char* filepath)
{
	char* buffer = nullptr;

	uint bytesFile = App->fm->Load(filepath, &buffer);

	const aiScene* scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	RELEASE_ARRAY(buffer);

	mesh tempMesh;
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* ourMesh = scene->mMeshes[i];
			tempMesh.num_vertex = ourMesh->mNumVertices;
			tempMesh.vertex = new float[tempMesh.num_vertex * 3];
			memcpy(tempMesh.vertex, ourMesh->mVertices, sizeof(float) * tempMesh.num_vertex * 3);
			LOG("New mesh with %d vertices", tempMesh.num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				tempMesh.num_index = ourMesh->mNumFaces * 3;
				tempMesh.index = new uint[tempMesh.num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&tempMesh.index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
		}
		aiReleaseImport(scene);
		
		meshes.push_back(tempMesh);
	}
	else 
	{
		LOG("Error loading scene %s", filepath);
	}
}

void ModuleImport::RenderMesh(mesh* m) {

	glGenBuffers(1, (GLuint*) & (vertex_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_vertex * 3, m->vertex, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glGenBuffers(1, (GLuint*) & (index_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m->num_index, m->index, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glDrawElements(GL_TRIANGLES, m->num_index, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &index_buffer);
}

uint ModuleImport::LoadTexture(const char* path)
{
	uint Image = 0;

	ilGenImages(1, &Image);
	ilBindImage(Image);

	//-------------
	ilLoadImage(path);

	uint ret = ilutGLBindTexImage();
	ilDeleteImages(1, &Image);

	//----------------------- 
	//char* buffer = nullptr;

	//uint bytesFile = App->physFS->Load(path, &buffer);

	//ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, bytesFile);
	//uint ret = ilutGLBindTexImage();

	//RELEASE_ARRAY(buffer);

	return ret;
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

update_status ModuleImport::PostUpdate(float dt)
{
	std::vector<mesh>::iterator ptr;

	for (ptr = meshes.begin(); ptr < meshes.end(); ptr++) 
	{
		mesh temp = *ptr;
		if(temp.enabled) RenderMesh(&temp);
	}
	return UPDATE_CONTINUE;
}