#include "Component_Mesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleJson.h"
#include "FileManager.h"
#include "Component_Transform.h"
#include "ResourceMesh.h"
#include "Component_Material.h"
#include "GameObject.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Glew/include/glew.h"


Component_Mesh::Component_Mesh() : Component(ComponentType::MESH), enableFaceNormals(false), enableVertexNormals(false), name("No name"), meshResource(nullptr)
{}

Component_Mesh::~Component_Mesh() 
{
	if (meshResource != nullptr) 
	{
		App->resources->ReleaseResource(meshResource->GetUID());
		meshResource = nullptr;
	}
}

void Component_Mesh::Update()
{
	Render();
}

void Component_Mesh::Render()
{
	if (!App->resources->ExistsResource(resourceUID)) {
		meshResource = nullptr;
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, meshResource->verticesBuffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_NORMAL_ARRAY, meshResource->normalsBuffer);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, meshResource->texcoordsBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshResource->indicesBuffer);

	glPushMatrix();
	glMultMatrixf((float*)&ownerGameObject->GetTransform()->GetGlobalTransform().Transposed());

	Component_Material* material = ownerGameObject->GetComponent<Component_Material>();

	if (material != nullptr && material->IsEnabled())
		material->BindTexture();

	glDrawElements(GL_TRIANGLES, meshResource->amountIndices, GL_UNSIGNED_INT, NULL);

	if (enableVertexNormals || App->renderer3D->drawVertexNormals)
		RenderVertexNormals();

	if (enableFaceNormals || App->renderer3D->drawFaceFormals)
		RenderFaceNormals();

	glPopMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Component_Mesh::OnGUI()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled", &enabled);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Mesh name: ");
		ImGui::SameLine();
		ImGui::Button(meshResource->name.c_str());
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESHES"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				SetResourceUID(payload_n);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Spacing();

		ImGui::Text("Assets path: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "%s", meshResource->assetsFile.c_str());
		ImGui::Text("Library path: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "%s", meshResource->libraryFile.c_str());
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "%d", meshResource->amountVertices);
		ImGui::Text("Indices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.95f, 0.5f, 0.07f, 1.0f), "%d", meshResource->amountIndices);
		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &enableVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &enableFaceNormals);
		ImGui::Spacing();

		ImGui::Text("UID: %d", resourceUID);
	}
}

void Component_Mesh::Save(JsonArray& saveArray)
{
	JsonObj saveObject;

	saveObject.AddInt("Type", type);
	saveObject.AddInt("MeshID", meshResource->GetUID());
	saveArray.AddObject(saveObject);
}

void Component_Mesh::Load(JsonObj& loadObject)
{
	int meshUID = loadObject.GetInt("MeshID");
	SetResourceUID(meshUID);
}

Resource* Component_Mesh::GetResource(ResourceType type)
{
	return meshResource;
}

void Component_Mesh::SetResourceUID(uint UID)
{
	resourceUID = UID;
	meshResource = (ResourceMesh*)App->resources->RequestResource(resourceUID);
	GenerateAABB();
}

void Component_Mesh::RenderVertexNormals()
{
	if (meshResource->normalsBuffer == -1) 
	{
		return;
	}

	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	float linesLength = 0.3f;

	for (int i = 0, c = 0; i < meshResource->amountVertices * 3; i += 3, c+= 4)
	{
		glColor3f(0.0f, 0.85f, 0.85f);
		glVertex3f(meshResource->vertices[i], meshResource->vertices[i + 1], meshResource->vertices[i + 2]);

		glVertex3f(meshResource->vertices[i] + (meshResource->normals[i] * linesLength),
			       meshResource->vertices[i + 1] + (meshResource->normals[i + 1] * linesLength),
			       meshResource->vertices[i + 2] + (meshResource->normals[i + 2]) * linesLength);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
	glEnable(GL_LIGHTING);
}

void Component_Mesh::RenderFaceNormals()
{
	if (meshResource->normalsBuffer == -1) 
	{
		return;
	}
		
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	float linesLength = 0.3f;

	for (int i = 0; i < meshResource->amountVertices * 3; i += 3)
	{
		glColor3f(1.0f, 0.85f, 0.0f);
		float vx = (meshResource->vertices[i] + meshResource->vertices[i + 3] + meshResource->vertices[i+ 6]) / 3;
		float vy = (meshResource->vertices[i + 1] + meshResource->vertices[i + 4] + meshResource->vertices[i + 7]) / 3;
		float vz = (meshResource->vertices[i + 2] + meshResource->vertices[i + 5] + meshResource->vertices[i + 8]) / 3;

		float nx = (meshResource->normals[i] +     meshResource->normals[i + 3] + meshResource->normals[i + 6]) / 3;
		float ny = (meshResource->normals[i + 1] + meshResource->normals[i + 4] + meshResource->normals[i + 7]) / 3;
		float nz = (meshResource->normals[i + 2] + meshResource->normals[i + 5] + meshResource->normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (meshResource->normals[i] * linesLength),
			       vy + (meshResource->normals[i + 1] * linesLength),
			       vz + (meshResource->normals[i + 2]) * linesLength);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
	glEnable(GL_LIGHTING);
}

PrimitiveGrid::PrimitiveGrid(int gridSize) 
{
	if ((gridSize % 2) != 0)
		gridSize++;

	size = gridSize;
}

PrimitiveGrid::~PrimitiveGrid() {}

void PrimitiveGrid::Render()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	for (float z = -size * 0.5f; z <= size * 0.5f; z++)
	{
		glVertex3f(-size * 0.5f, 0, z);
		glVertex3f(size * 0.5f, 0, z);
	}
	for (float x = -size * 0.5f; x <= size * 0.5f; x++)
	{	
		glVertex3f(x, 0, -size * 0.5f);
		glVertex3f(x, 0, size * 0.5f);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void Component_Mesh::GenerateAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose((float3*)meshResource->vertices, meshResource->amountVertices);
}

AABB Component_Mesh::GetAABB()
{
	return aabb;
}
