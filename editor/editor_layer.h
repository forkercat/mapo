//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "engine/layer.h"

#include "engine/scene/editor_camera.h"

#include "engine/event/key_event.h"
#include "engine/event/mouse_event.h"

#include "editor/panel/scene_panel.h"
#include "editor/panel/info_panel.h"

class ImVec2;

namespace Mapo
{
	class SimpleRenderSystem;
	class PointLightSystem;
	class Buffer;
	class Scene;

	class EditorLayer : public Layer
	{
	public:
		virtual ~EditorLayer() = default;

		EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void Init();
		void CreateScene();

		// Event callbacks
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

		void NewScene() { MP_INFO("NewScene()"); }
		void OpenScene() { MP_INFO("OpenScene()"); }
		void SaveSceneAs() { MP_INFO("SaveSceneAs()"); }

		void DisplayMenuBar();

		void OnGizmoUpdate();
		void DrawGizmoControls();

		void FocusOnGameObject();

		// Testing
		void ShowCameraMatrix();

	private:
		Ref<Scene> m_scene;

		EditorCamera m_camera;

		int m_gizmoType{ INVALID_GIZMO_TYPE };

		// Systems
		UniqueRef<SimpleRenderSystem> m_renderSystem{};
		UniqueRef<PointLightSystem>	  m_pointLightSystem{};

		// Panels
		ScenePanel m_scenePanel;
		InfoPanel  m_infoPanel;
	};

} // namespace Mapo
