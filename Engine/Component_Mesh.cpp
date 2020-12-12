#include "Component_Mesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "FileManager.h"
#include "Component_Material.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "ModuleJson.h"

#include "ResourceMesh.h"

#include "glew/include/glew.h"
#include "ImGui/imgui.h"

Component_Mesh::Component_Mesh() : Component(ComponentType::MESH), drawFaceFormals(false), drawVertexNormals(false), name("No name"), meshResource(nullptr)
{}

Component_Mesh::~Component_Mesh() 
{
	if (meshResource != nullptr) 
	{
		App->resources->ReleaseResource(meshResource->GetUID());
		meshResource = nullptr;
	}
}

void Component_Mesh::Save(JsonArray& save_array)
{
	JsonObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddInt("MeshID", meshResource->GetUID());

	save_array.AddObject(save_object);
}

void Component_Mesh::Load(JsonObj& load_object)
{
	int meshUID = load_object.GetInt("MeshID");
	SetResourceUID(meshUID);
}

void Component_Mesh::SetResourceUID(uint UID)
{
	_resourceUID = UID;
	meshResource = (ResourceMesh*)App->resources->RequestResource(_resourceUID);
	GenerateAABB();
}

Resource* Component_Mesh::GetResource(ResourceType type)
{
	return meshResource;
}

void Component_Mesh::Update()
{
 	Render();
}

void Component_Mesh::Render()
{
	if (!App->resources->Exists(_resourceUID)) {
		meshResource = nullptr;
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, meshResource->verticesBuffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//normals
	glBindBuffer(GL_NORMAL_ARRAY, meshResource->normalsBuffer);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//textures
	glBindBuffer(GL_ARRAY_BUFFER, meshResource->texcoordsBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshResource->indicesBuffer);

	glPushMatrix();
	glMultMatrixf((float*)&ownerGameObject->GetTransform()->GetGlobalTransform().Transposed());

	Component_Material* material = ownerGameObject->GetComponent<Component_Material>();

	if (material != nullptr)
		material->BindTexture();

	glDrawElements(GL_TRIANGLES, meshResource->amountIndices, GL_UNSIGNED_INT, NULL);

	if(drawVertexNormals ||App->renderer3D->drawVertexNormals)
		DrawVertexNormals();

	if (drawFaceFormals || App->renderer3D->drawFaceFormals)
		DrawFaceNormals();

	glPopMatrix();

	//clean buffers
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

		ImGui::Text("Assets path: %s", meshResource->assetsFile.c_str());
		ImGui::Text("Library path: %s", meshResource->libraryFile.c_str());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Vertices: %d Indices: %d", meshResource->amountVertices, meshResource->amountIndices);
		ImGui::Spacing();

		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &drawVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &drawFaceFormals);

		ImGui::Spacing();
		ImGui::Text("UID: %d", _resourceUID);
		ImGui::Spacing();
	}
}

void Component_Mesh::DrawVertexNormals()
{
	if (meshResource->normalsBuffer == -1)
		return;

	float normal_lenght = 0.3f;

	//vertices normals
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (size_t i = 0, c = 0; i < meshResource->amountVertices * 3; i += 3, c+= 4)
	{
		glColor3f(0.0f, 0.85f, 0.85f);
		//glColor4f(colors[c], colors[c + 1], colors[c + 2], colors[c + 3]);
		glVertex3f(meshResource->vertices[i], meshResource->vertices[i + 1], meshResource->vertices[i + 2]);

		glVertex3f(meshResource->vertices[i] + (meshResource->normals[i] * normal_lenght),
			       meshResource->vertices[i + 1] + (meshResource->normals[i + 1] * normal_lenght),
			       meshResource->vertices[i + 2] + (meshResource->normals[i + 2]) * normal_lenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
	glEnable(GL_LIGHTING);
}

void Component_Mesh::DrawFaceNormals()
{
	if (meshResource->normalsBuffer == -1)
		return;

	float normal_lenght = 0.3f;

	//vertices normals
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (size_t i = 0; i < meshResource->amountVertices * 3; i += 3)
	{
		glColor3f(1.0f, 0.85f, 0.0f);
		float vx = (meshResource->vertices[i] + meshResource->vertices[i + 3] + meshResource->vertices[i+ 6]) / 3;
		float vy = (meshResource->vertices[i + 1] + meshResource->vertices[i + 4] + meshResource->vertices[i + 7]) / 3;
		float vz = (meshResource->vertices[i + 2] + meshResource->vertices[i + 5] + meshResource->vertices[i + 8]) / 3;

		float nx = (meshResource->normals[i] +     meshResource->normals[i + 3] + meshResource->normals[i + 6]) / 3;
		float ny = (meshResource->normals[i + 1] + meshResource->normals[i + 4] + meshResource->normals[i + 7]) / 3;
		float nz = (meshResource->normals[i + 2] + meshResource->normals[i + 5] + meshResource->normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (meshResource->normals[i] * normal_lenght),
			       vy + (meshResource->normals[i + 1] * normal_lenght),
			       vz + (meshResource->normals[i + 2]) * normal_lenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
	glEnable(GL_LIGHTING);
}

// PrimitiveGrid =========================================================================================================================

PrimitiveGrid::PrimitiveGrid(int g_size) 
{
	if ((g_size % 2) != 0)
		g_size++;

	size = g_size;
}

PrimitiveGrid::~PrimitiveGrid() {}

void PrimitiveGrid::Render()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	//Vertical Lines
	for (float x = -size * 0.5f; x <= size * 0.5f; x++)
	{	
		glVertex3f(x, 0, -size * 0.5f);
		glVertex3f(x, 0, size * 0.5f);
	}

	//Hortiontal Lines
	for (float z = -size * 0.5f; z <= size * 0.5f; z++)
	{
		glVertex3f(-size * 0.5f, 0, z);
		glVertex3f(size * 0.5f, 0, z);
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
