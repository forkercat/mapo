//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "engine/layer.h"
#include "engine/scene.h"
#include "engine/camera.h"
#include "engine/keyboard_controller.h"

#include "engine/event/key_event.h"
#include "engine/event/mouse_event.h"

namespace Mapo
{
	class ImGuiSystem;
	class SimpleRenderSystem;
	class PointLightSystem;
	class RainbowSystem;

	class Buffer;

	class EditorLayer : public Layer
	{
	public:
		virtual ~EditorLayer() = default;

		EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(const Timestep dt) override;
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

	private:
		UniqueRef<Scene> m_scene;

		Camera			   m_camera;
		KeyboardController m_controller;

		GameObject m_player;

		// Systems
		UniqueRef<SimpleRenderSystem> m_renderSystem{};
		UniqueRef<PointLightSystem>	  m_pointLightSystem{};
		UniqueRef<RainbowSystem>	  m_rainbowSystem{};
	};

} // namespace Mapo
