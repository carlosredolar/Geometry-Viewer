#include "GuiScene.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "glew/include/glew.h"
#include "GuiAssets.h"
#include "Time.h"


GuiScene::GuiScene() : GuiWindow()
{
	type = WindowType::SCENE_WINDOW;
}

GuiScene::~GuiScene()
{
}

void GuiScene::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Scene", &visible, ImGuiWindowFlags_MenuBar))
	{
		App->gui->sceneWindowFocused = ImGui::IsWindowFocused();

		if (ImGui::BeginMenuBar())
		{
			static bool vertex_normals = App->renderer3D->drawVertexNormals;
			if (ImGui::Checkbox("Vertex Normals", &vertex_normals))
				App->renderer3D->drawVertexNormals = vertex_normals;

			static bool face_normals = App->renderer3D->drawFaceFormals;
			if (ImGui::Checkbox("Face Normals", &face_normals))
				App->renderer3D->drawFaceFormals = face_normals;

			static bool showGrid = App->scene->showGrid;
			if (ImGui::Checkbox("Show Grid", &showGrid))
				App->scene->showGrid = showGrid;

			static bool showBB = App->scene->showBB;
			if (ImGui::Checkbox("Show Bounding Boxes", &showBB))
				App->scene->showBB = showBB;

			ImGui::Checkbox("Camera Culling", &App->renderer3D->cameraCulling);

			ImGui::EndMenuBar();
		}

		ImVec2 window_size = ImGui::GetContentRegionAvail();
		App->gui->sceneWindowOrigin = ImGui::GetWindowPos();
		App->gui->sceneWindowOrigin.x += ImGui::GetWindowContentRegionMin().x;
		App->gui->sceneWindowOrigin.y += ImGui::GetWindowContentRegionMin().y;

		App->gui->mouseScenePosition.x = App->input->GetMouseX() - App->gui->sceneWindowOrigin.x;
		App->gui->mouseScenePosition.y = App->input->GetMouseY() - App->gui->sceneWindowOrigin.y;

		if (App->in_game)
			DrawInGameDataOverlay();

		if (App->gui->image_size.x != window_size.x || App->gui->image_size.y != window_size.y)
			App->gui->OnResize(window_size);

		ImGui::Image((ImTextureID)App->renderer3D->colorTexture, App->gui->image_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::PushID(SCENE_WINDOW);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				GuiAssets* assets_window = (GuiAssets*)App->gui->windows[ASSETS_WINDOW];
				const char* file = assets_window->GetFileAt(payload_n);
				App->scene->AddGameObject(App->resources->RequestGameObject(file));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();

		App->scene->EditTransform();
		//ImGui::Image((ImTextureID)App->renderer3D->depthTexture, image_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void GuiScene::DrawInGameDataOverlay()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	window_flags |= ImGuiWindowFlags_NoMove;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 window_pos = App->gui->sceneWindowOrigin;
	window_pos.x += 10.0f;
	window_pos.y += 10.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
	ImGui::SetNextWindowPos(window_pos);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.25f, 0.25f, 0.25f, 0.75f));
	bool dummy_bool = true;
	if (ImGui::Begin("Example: Simple overlay", &dummy_bool, window_flags))
	{
		ImGui::Text("Game Time");
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Delta time %.3f", Time::gameClock.dt);
		ImGui::Text("Time Scale: %.2f", Time::gameClock.timeScale);
		ImGui::Text("Time since game start: %.2f", Time::gameClock.timeSinceStartup());
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::End();
}
