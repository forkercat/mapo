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

namespace Mapo
{
	ScenePanel::ScenePanel(Ref<Scene> scene)
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
		ImGui::Begin("Test");

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

		ImGui::Text("Stats");
		ImGui::Separator();
		ImGui::Text("FPS: %.1f  (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

		Window& window = Application::Get().GetWindow();
		ImGui::Text("Window: %u x %u", window.GetWidth(), window.GetHeight());

		Renderer& renderer = RenderContext::GetRenderer();
		ImGui::Text("Swapchain: %u x %u", renderer.GetSwapchainWidth(), renderer.GetSwapchainHeight());

		ImGui::Text("Image count: %u", renderer.GetImageCount());

		ImGui::NewLine();
		ImGui::Separator();
		static F32 color[3]{ 1.0f, 0.0f, 0.0f };
		ImGui::ColorEdit3("Test color", color);

		ImGui::End(); // Test
	}

} // namespace Mapo
