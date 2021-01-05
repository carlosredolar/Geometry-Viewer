#include "Component_CanvasRenderer.h"
#include "Component_Transform.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Glew/include/glew.h"

Component_CanvasRenderer::Component_CanvasRenderer(GameObject* parent) : Component(CANVASRENDERER, parent)
{

}

Component_CanvasRenderer::~Component_CanvasRenderer()
{

}

void Component_CanvasRenderer::OnGUI()
{
	if (ImGui::CollapsingHeader("Canvas Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled ", &enabled);
	}
}
void Component_CanvasRenderer::Save(JsonArray& saveArray)
{

}
void Component_CanvasRenderer::Load(JsonObj& loadObject)
{

}

void Component_CanvasRenderer::DrawGraphic(ResourceTexture* texture)
{
	if (amountVertices > 3)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		//glBindBuffer(GL_NORMAL_ARRAY, normalsBuffer);
		//glNormalPointer(GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, texcoordsBuffer);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

		glPushMatrix();
		glMultMatrixf((float*)& ownerGameObject->GetTransform()->GetGlobalTransform().Transposed());

		if (texture != nullptr)
			texture->BindTexture();

		glDrawElements(GL_TRIANGLES, amountVertices, GL_UNSIGNED_INT, NULL);

		glPopMatrix();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_NORMAL_ARRAY, 0);
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void Component_CanvasRenderer::SetGraphic(Component_Graphic* set_graphic)
{
	graphic = set_graphic;
}