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

#include <imgui/imgui.h>
#include <IconFontCppHeaders/IconsFontAwesome5.h>

namespace Mapo
{
	ScenePanel::ScenePanel()
		: Panel("Scene")
	{
	}

	ScenePanel::ScenePanel(Ref<Scene> scene)
		: Panel("Scene")
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
		Renderer& renderer = RenderContext::GetRenderer();

		// ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;

		ImGui::Begin(GetPanelName().c_str());

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
		ImGui::ColorEdit3("Clear color", GLM_PTR(renderer.ClearColor()));

		ImGui::Button(ICON_FA_SEARCH " Search");

		ImGui::End(); // Test
	}

	void ScenePanel::DrawHierarchy()
	{

	}

	void ScenePanel::DrawInspector()
	{

	}

} // namespace Mapo
