//
// Created by Junhao Wang (@forkercat) on 5/18/24.
//

#pragma once

#include "editor/panel/panel.h"

#include "engine/scene/scene.h"
#include "engine/scene/game_object.h"

namespace Mapo
{
	class EditorCamera;

	class ScenePanel : public Panel
	{
	public:
		virtual ~ScenePanel() = default;

		ScenePanel();
		ScenePanel(Ref<Scene> scene);

		void SetContext(Ref<Scene> context);

		void OnImGuiRender(EditorCamera& camera);

		GameObject& GetSelection() { return m_selectedGameObject; };

	private:
		// Scene Hierarchy
		void DrawHierarchy(EditorCamera& camera);
		void DrawGameObjectNode(GameObject& gameObject, EditorCamera& camera);

		// Inspector
		void DrawSelectedGameObject();
		void DrawComponents(GameObject& gameObject);

		// Menu
		void DrawGameObjectCreationMenu();

	private:
		Ref<Scene> m_scene;

		GameObject m_selectedGameObject{};
	};
} // namespace Mapo
