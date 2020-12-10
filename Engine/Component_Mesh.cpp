#include "Component_Mesh.h"
#include "Component_Material.h"
#include "Component_Transform.h"
#include "ModuleImport.h"
#include "ModuleJson.h"
#include "Glew/include/glew.h"

#include "Application.h"

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
	if (mesh != nullptr)
	{
		App->resources->ReleaseResource(mesh->GetUID());
		mesh = nullptr;
	}
}

void Component_Mesh::Update()
{
	if(IsEnabled()) 
		Render();
}

void Component_Mesh::Render() 
{
	//Render

	if (!App->resources->Exists(resourceUID)) {
		mesh = nullptr;
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glEnableClientState(GL_NORMAL_ARRAY);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->verticesBuffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->normalsBuffer);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->textureCoordsBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer);

	float4x4 localTransform = this->ownerGameObject->GetComponent<Component_Transform>()->GetGlobalTransform().Transposed();
	glPushMatrix();
	glMultMatrixf((float*)& localTransform);

	Component_Material* material = ownerGameObject->GetComponent<Component_Material>();

	if (material && material->IsEnabled())
	{
		glEnableClientState(GL_TEXTURE_2D);
		material->BindTexture();
	}
	
	glDrawElements(GL_TRIANGLES, mesh->amountIndices, GL_UNSIGNED_INT, NULL);

	if (enableVertexNormals) {

		RenderVertexNormals();
	}

	if (enableFaceNormals) {

		RenderFaceNormals();
	}

	glPopMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Component_Mesh::RenderVertexNormals()
{
	if (mesh->normalsBuffer == -1)
		return;

	float normalLenght = 0.5f;

	//vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0, c = 0; i < mesh->amountVertices * 3; i += 3, c += 4)
	{
		glColor3f(0.0f, 0.85f, 0.85f);
		//glColor4f(colors[c], colors[c + 1], colors[c + 2], colors[c + 3]);
		glVertex3f(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]);

		glVertex3f(mesh->vertices[i] + (mesh->normals[i] * normalLenght),
			mesh->vertices[i + 1] + (mesh->normals[i + 1] * normalLenght),
			mesh->vertices[i + 2] + (mesh->normals[i + 2]) * normalLenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

void Component_Mesh::RenderFaceNormals()
{
	if (mesh->normalsBuffer == -1)
		return;

	float normalLenght = 0.5f;

	//vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0; i < mesh->amountVertices * 3; i += 3)
	{
		glColor3f(1.0f, 0.85f, 0.0f);
		float vx = (mesh->vertices[i] + mesh->vertices[i + 3] + mesh->vertices[i + 6]) / 3;
		float vy = (mesh->vertices[i + 1] + mesh->vertices[i + 4] + mesh->vertices[i + 7]) / 3;
		float vz = (mesh->vertices[i + 2] + mesh->vertices[i + 5] + mesh->vertices[i + 8]) / 3;

		float nx = (mesh->normals[i] + mesh->normals[i + 3] + mesh->normals[i + 6]) / 3;
		float ny = (mesh->normals[i + 1] + mesh->normals[i + 4] + mesh->normals[i + 7]) / 3;
		float nz = (mesh->normals[i + 2] + mesh->normals[i + 5] + mesh->normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (mesh->normals[i] * normalLenght),
			vy + (mesh->normals[i + 1] * normalLenght),
			vz + (mesh->normals[i + 2]) * normalLenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}

void Component_Mesh::SetResourceUID(uint UID)
{
	resourceUID = UID;
	mesh = (ResourceMesh*)App->resources->RequestResource(resourceUID);
	if (mesh != nullptr)
		GenerateAABB();
}

Resource* Component_Mesh::GetResource(ResourceType type)
{
	return mesh;
}

void Component_Mesh::GenerateAABB()
{
	AABB.SetNegativeInfinity();
	AABB.Enclose((float3*)mesh->vertices, mesh->amountVertices);
}

AABB Component_Mesh::GetAABB()
{
	return AABB;
}

const char* Component_Mesh::GetName() 
{
	return mesh->name.c_str();
}

int Component_Mesh::GetVertices()
{
	return mesh->amountVertices;
}

int Component_Mesh::GetIndices()
{
	return mesh->amountIndices;
}

void Component_Mesh::OnGUI()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled", &enabled);

		ImGui::Separator();
		ImGui::Spacing();

		std::string meshID = "Mesh: ";
		meshID.append(mesh->name);
		ImGui::Button(meshID.c_str());

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("MESHES"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				SetResourceUID(payload_n);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Spacing();

		ImGui::Text("Assets path: %s", mesh->assetsFile.c_str());
		ImGui::Text("Library path: %s", mesh->libraryFile.c_str());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Vertices: %d Indices: %d", mesh->amountVertices, mesh->amountIndices);
		ImGui::Spacing();



		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &enableVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &enableFaceNormals);

		ImGui::Spacing();
	}
}

void Component_Mesh::Save(JsonArray& save_array)
{
	JsonObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddInt("MeshID", mesh->GetUID());

	save_array.AddObject(save_object);
}

void Component_Mesh::Load(JsonObj& load_object)
{
	int meshUID = load_object.GetInt("MeshID");
	SetResourceUID(meshUID);
}