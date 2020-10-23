#include "Globals.h"
#include "Application.h"
#include "ModuleImport.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}


ModuleImport::~ModuleImport()
{

}

bool ModuleImport::Init() {

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

update_status ModuleImport::Update() {


	return UPDATE_CONTINUE;
}


bool ModuleImport::CleanUp() {

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleImport::LoadMesh(char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* ourMesh = scene->mMeshes[i];
			myMesh.num_vertex = ourMesh->mNumVertices;
			myMesh.vertex = new float[myMesh.num_vertex * 3];
			memcpy(myMesh.vertex, ourMesh->mVertices, sizeof(float) * myMesh.num_vertex * 3);
			LOG("New mesh with %d vertices", myMesh.num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				myMesh.num_index = ourMesh->mNumFaces * 3;
				myMesh.index = new uint[myMesh.num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&myMesh.index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
		}

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", file_path);
}

void ModuleImport::RenderMesh(mesh m) {
	uint vertex_buffer = 0;
	glGenBuffers(1, (GLuint*) & (vertex_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m.num_vertex * 3, m.vertex, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	uint index_buffer = 0;
	glGenBuffers(1, (GLuint*) & (index_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m.num_index, m.index, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glDrawElements(GL_TRIANGLES, m.num_index, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}