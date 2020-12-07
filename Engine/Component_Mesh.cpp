#include "Component_Mesh.h"
#include "Component_Material.h"
#include "Component_Transform.h"
#include "ModuleImport.h"
#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

Component_Mesh::Component_Mesh(GameObject* ownerGameObject, bool enabled) : Component(ComponentType::MESH, ownerGameObject, enabled)
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

	Component_Material* texture = ownerGameObject->GetComponent<Component_Material>();

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

	float4x4 localTransform = this->ownerGameObject->GetComponent<Component_Transform>()->GetGlobalTransform().Transposed();
	glPushMatrix();
	glMultMatrixf((float*)& localTransform);

	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, NULL);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);



	if (enableVertexNormals) {

		RenderVertexNormals(mesh->vertices, mesh->normals);
	}

	if (enableFaceNormals) {

		RenderFaceNormals(mesh->vertices, mesh->normals);
	}
}

void Component_Mesh::RenderVertexNormals(std::vector<float3> vertices, std::vector<float3> normals)
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.7f, 0.0f);

	for (size_t i = 0; i < vertices.size(); i++)
	{
		float x = vertices[i].x;
		float y = vertices[i].y;
		float z = vertices[i].z;
		glVertex3f(x, y, z);

		float normal_x = normals[i].x/3;
		float normal_y = normals[i].y/3;
		float normal_z = normals[i].z/3;
		glVertex3f(x + normal_x, y + normal_y, z + normal_z);
	}
	glColor3f(1.f, 1.f, 1.f);
	glEnd();
}

void Component_Mesh::RenderFaceNormals(std::vector<float3> vertices, std::vector<float3> normals)
{
	glBegin(GL_LINES);
	glColor3f(0.7f, 0.0f, 0.0f);

	for (size_t i = 0; i < vertices.size()-2 ; i += 3)
	{
		float x = (vertices[i].x + vertices[i + 1].x + vertices[i + 2].x) / 3;
		float y = (vertices[i].y + vertices[i + 1].y + vertices[i + 2].y) / 3;
		float z = (vertices[i].z + vertices[i + 1].z + vertices[i + 2].z) / 3;
		glVertex3f(x, y, z);

		float normal_x = normals[i].x/3;
		float normal_y = normals[i].y/3;
		float normal_z = normals[i].z/3;
		glVertex3f(x + normal_x, y + normal_y, z + normal_z);
	}
	glColor3f(1.f, 1.f, 1.f);
	glEnd();

}

const char* Component_Mesh::GetName() 
{
	return mesh->name.c_str();
}

int Component_Mesh::GetVertices()
{
	return mesh->vertices.size();
}

int Component_Mesh::GetIndices()
{
	return mesh->indices.size();
}