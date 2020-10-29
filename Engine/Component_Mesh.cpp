#include "Component_Mesh.h"
#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

Component_Mesh::Component_Mesh(GameObject* ownerGameObject, bool enabled) : Component(COMPONENT_TYPE::MESH, ownerGameObject, enabled)
{

}

Component_Mesh::~Component_Mesh()
{
	CleanUp();
}

void Component_Mesh::Update()
{
	Render();
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
	vertices.clear();
	indices.clear();
	normals.clear();
	textureCoords.clear();

	glDeleteBuffers(1, &idVertex);
	glDeleteBuffers(1, &idIndex);
	glDeleteBuffers(1, &idNormals);
	glDeleteBuffers(1, &idTextureCoords);
}

void Component_Mesh::GenerateMesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords)
{
	this->vertices = vertices;
	this->indices = indices;
	this->normals = normals;
	this->textureCoords = textureCoords;

	CreateBuffers();
}

void Component_Mesh::CreateBuffers()
{
	if (!indices.empty() && !vertices.empty())
	{
		glGenBuffers(1, (GLuint*) & (idVertex));
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*) & (idIndex));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

		if (!normals.empty())
		{
			glGenBuffers(1, (GLuint*) & (idNormals));
			glBindBuffer(GL_ARRAY_BUFFER, idNormals);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float3), &normals[0], GL_STATIC_DRAW);
		}

		if (!textureCoords.empty())
		{
			glGenBuffers(1, (GLuint*) & (idTextureCoords));
			glBindBuffer(GL_ARRAY_BUFFER, idTextureCoords);
			glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float2), &textureCoords[0], GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
}

void Component_Mesh::Render() {
	//Render
	glEnableClientState(GL_VERTEX_ARRAY);

	if (idNormals != -1)
		glEnableClientState(GL_NORMAL_ARRAY);

	if (idTextureCoords != -1)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (idTextureImage != -1)
	{
		glEnableClientState(GL_TEXTURE_2D);
	}

	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (idNormals != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, idNormals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (idTextureCoords != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, idTextureCoords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	if (idTextureImage != -1)
	{
		glBindTexture(GL_TEXTURE_2D, idTextureImage);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);


	if (enableVertexNormals) {

		RenderVertexNormals(vertices, normals);
	}

	if (enableFaceNormals) {

		RenderFaceNormals(vertices, normals);
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

		float normal_x = normals[i].x;
		float normal_y = normals[i].y;
		float normal_z = normals[i].z;
		glVertex3f(x + normal_x, y + normal_y, z + normal_z);
	}

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

		float normal_x = normals[i].x;
		float normal_y = normals[i].y;
		float normal_z = normals[i].z;
		glVertex3f(x + normal_x, y + normal_y, z + normal_z);
	}

	glEnd();
}