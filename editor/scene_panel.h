//
// Created by Junhao Wang (@forkercat) on 5/18/24.
//

#pragma once

#include "engine/scene/scene.h"
#include "engine/scene/game_object.h"

namespace Mapo
{
	class EditorCamera;

	class ScenePanel
	{
	public:
		virtual ~ScenePanel() = default;

		ScenePanel() = default;
		ScenePanel(Ref<Scene> scene);

		void SetContext(Ref<Scene> context);

		void OnImGuiRender(EditorCamera& camera);

	private:

	private:
		Ref<Scene> m_scene;

		GameObject m_selectedGameObject{};
	};
} // namespace Mapo
