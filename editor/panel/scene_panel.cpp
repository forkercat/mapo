//
// Created by Junhao Wang (@forkercat) on 5/18/24.
//

#include "scene_panel.h"

#include "engine/application.h"
#include "engine/window.h"

#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"

#include "engine/scene/editor_camera.h"
#include "engine/scene/game_object.h"
#include "engine/scene/component.h"

#include "3rdparty/imgui/imgui.h"

namespace Mapo
{
	ScenePanel::ScenePanel() : Panel("Scene")
	{
	}

	ScenePanel::ScenePanel(Ref<Scene> scene) : Panel("Scene")
	{
		m_scene = scene;
	}

	void ScenePanel::SetContext(Ref<Scene> context)
	{
		m_scene = context;
		m_selectedGameObject = {};
	}

	void ScenePanel::OnImGuiRender(EditorCamera& camera)
	{
		// Test
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;

		ImGui::Begin(GetPanelName().c_str(), nullptr, flags);

		static bool showDemo = true;
		ImGui::Checkbox("Show demo", &showDemo);
		if (showDemo)
		{
			ImGui::ShowDemoWindow(&showDemo);
		}

		ImGui::NewLine();

		ImGui::Text("Editor Camera");
		ImGui::Separator();
		ImGui::DragFloat3("Position##1", GLM_PTR(camera.GetPosition()));

		auto gameObjects = m_scene->GetGameObjects();

		ImGui::NewLine();

		ImGui::Text("Game Objects");
		ImGui::Separator();

		for (GameObject& go : gameObjects)
		{
			auto& transform = go.GetComponent<TransformComponent>();
			ImGui::DragFloat3(go.GetName().c_str(), GLM_PTR(transform.translation));
		}

		ImGui::NewLine();
		ImGui::Separator();
		static F32 color[3]{ 1.0f, 0.0f, 0.0f };
		ImGui::ColorEdit3("Test color", color);

		ImGui::End(); // Test
	}

	void ScenePanel::DrawHierarchy()
	{

	}

	void ScenePanel::DrawInspector()
	{

	}

} // namespace Mapo
