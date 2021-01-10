#include "Component_CanvasRenderer.h"
#include "Component_Transform.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Color.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Glew/include/glew.h"

Component_CanvasRenderer::Component_CanvasRenderer(GameObject* parent) : Component(CANVASRENDERER, parent)
{
	vertices = new float[8];
}

Component_CanvasRenderer::~Component_CanvasRenderer()
{
	delete vertices;
}

void Component_CanvasRenderer::OnGUI()
{
	if (ImGui::CollapsingHeader("Canvas Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled", &enabled);
	}
}
void Component_CanvasRenderer::Save(JsonArray& saveArray)
{

}
void Component_CanvasRenderer::Load(JsonObj& loadObject)
{

}

void Component_CanvasRenderer::DrawGraphic(uint texture, Color color, bool inGame)
{
	if (enabled && vertices != nullptr)
	{
		glEnable(GL_BLEND);
		if (inGame) glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		glPushMatrix();
		glMultMatrixf((float*)& ownerGameObject->GetTransform()->GetGlobalTransform().Transposed());

		glColor4f(color.r, color.g, color.b, color.a * alphaCanvas);

		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(vertices[0], vertices[1]);
		glTexCoord2f(textureTile.x, 0); glVertex2f(vertices[2], vertices[3]);
		glTexCoord2f(textureTile.x, textureTile.y); glVertex2f(vertices[4], vertices[5]);
		glTexCoord2f(0, textureTile.y); glVertex2f(vertices[6], vertices[7]);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix();

		glEnable(GL_LIGHTING);
		if (inGame) glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}

float* Component_CanvasRenderer::GetVertices()
{
	return vertices;
}
void Component_CanvasRenderer::SetTextureTile(float2 texTile)
{
	textureTile = texTile;
}

void Component_CanvasRenderer::SetAlpha(float a)
{
	alphaCanvas = a;
}