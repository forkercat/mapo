//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "engine/layer.h"
#include "engine/scene.h"

namespace Mapo
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent() override;

	private:
		// bool OnKeyPressed()
		// bool OnMouseButtonPressed()

		// void NewScene();
		// void OpenScene();
		// void SaveSceneAs();

	private:
		UniqueRef<Scene> m_scene;
	};

} // namespace Mapo
