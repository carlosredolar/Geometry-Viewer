#include "ResourceMesh.h"
#include "Libs/Glew/include/glew.h"

ResourceMesh::ResourceMesh(uint UID) : Resource(UID, ResourceType::RESOURCE_MESH), amountVertices(0), vertices(nullptr), amountIndices(0), indices(nullptr),
 amountNormals(0), normals(nullptr), textureCoords(nullptr), amountTexCoords(0), colors(nullptr), buffersCreated(false) {}

ResourceMesh::~ResourceMesh()
{
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
	amountTexCoords = 0;

	if (buffersCreated) DeleteBuffers();
}

void ResourceMesh::Load(JsonObj& loadingObj) 
{}

void ResourceMesh::GenerateBuffers()
{
	//vertices
	glGenBuffers(1, (GLuint*)&(verticesBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * amountVertices * 3, vertices, GL_STATIC_DRAW);

	//normals
	glGenBuffers(1, (GLuint*)&(normalsBuffer));
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuffer);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * amountVertices * 3, normals, GL_STATIC_DRAW);

	//textures
	glGenBuffers(1, (GLuint*)&(texcoordsBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, texcoordsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * amountVertices * 2, textureCoords, GL_STATIC_DRAW);

	//indices
	glGenBuffers(1, (GLuint*)&(indicesBuffer));
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
	glDeleteBuffers(1, &texcoordsBuffer);
	texcoordsBuffer = 0;

	buffersCreated = false;
}
