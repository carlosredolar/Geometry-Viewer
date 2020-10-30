#include "Component_Mesh.h"
#include "Component_Texture.h"
#include "ModuleImport.h"
#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

Component_Mesh::Component_Mesh(GameObject* ownerGameObject, bool enabled) : Component(COMPONENT_TYPE::MESH, ownerGameObject, enabled)
{
	mesh = nullptr;
}

Component_Mesh::~Component_Mesh()
{
	CleanUp();
}

void Component_Mesh::Update()
{
	if(IsEnabled()) Render();
}

void Component_Mesh::Enable()
{
	enabled = true;
}

void Component_Mesh::Disable()
{
	enabled = false;
}

bool Component_Mesh::IsEnabled()
{
	return enabled;
}

void Component_Mesh::CleanUp() {
	mesh->vertices.clear();
	mesh->indices.clear();
	mesh->normals.clear();
	mesh->textureCoords.clear();

	glDeleteBuffers(1, &mesh->idVertex);
	glDeleteBuffers(1, &mesh->idIndex);
	glDeleteBuffers(1, &mesh->idNormals);
	glDeleteBuffers(1, &mesh->idTextureCoords);

	delete mesh;
}

void Component_Mesh::GenerateMesh(meshInfo* newMesh, std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords)
{
	mesh = newMesh;
	mesh->vertices = vertices;
	mesh->indices = indices;
	mesh->normals = normals;
	mesh->textureCoords = textureCoords;

	CreateBuffers();
}

void Component_Mesh::CreateBuffers()
{
	if (!mesh->indices.empty() && !mesh->vertices.empty())
	{
		glGenBuffers(1, (GLuint*) & (mesh->idVertex));
		glBindBuffer(GL_ARRAY_BUFFER, mesh->idVertex);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float3), &mesh->vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*) & (mesh->idIndex));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->idIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint), &mesh->indices[0], GL_STATIC_DRAW);

		if (!mesh->normals.empty())
		{
			glGenBuffers(1, (GLuint*) & (mesh->idNormals));
			glBindBuffer(GL_ARRAY_BUFFER, mesh->idNormals);
			glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(float3), &mesh->normals[0], GL_STATIC_DRAW);
		}

		if (!mesh->textureCoords.empty())
		{
			glGenBuffers(1, (GLuint*) & (mesh->idTextureCoords));
			glBindBuffer(GL_ARRAY_BUFFER, mesh->idTextureCoords);
			glBufferData(GL_ARRAY_BUFFER, mesh->textureCoords.size() * sizeof(float2), &mesh->textureCoords[0], GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
}

void Component_Mesh::Render() 
{
	//Render

	Component_Texture* texture = ownerGameObject->GetComponent<Component_Texture>();

	glEnableClientState(GL_VERTEX_ARRAY);

	if (mesh->idNormals != -1)
		glEnableClientState(GL_NORMAL_ARRAY);

	if (mesh->idTextureCoords != -1)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (texture && texture->IsEnabled())
	{
		glEnableClientState(GL_TEXTURE_2D);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mesh->idVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (mesh->idNormals != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->idNormals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (mesh->idTextureCoords != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->idTextureCoords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	if (texture && texture->IsEnabled())
	{
		glBindTexture(GL_TEXTURE_2D, texture->GetIdTexture());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->idIndex);

	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
}

const char* Component_Mesh::GetName() 
{
	return mesh->name.c_str();
}