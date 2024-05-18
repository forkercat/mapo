//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#pragma once

#include "core/core.h"

#include "engine/scene/editor_camera.h"

#include <entt/entity/registry.hpp>

#include <vector>

namespace Mapo
{
	class GameObject;

	class Scene
	{
	public:
		virtual ~Scene() = default;

		Scene();

		void OnUpdateRuntime(Timestep dt);
		void OnUpdateEditor(Timestep dt, EditorCamera& camera);

		GameObject CreateGameObject(const String& name = "");
		void	   DestroyGameObject(GameObject& gameObject);

		// TODO: Should not do this.
		std::vector<GameObject> GetGameObjects();

	private:
		template <typename T>
		void OnComponentAdded(GameObject& gameObject, T& component);

	private:
		entt::registry m_registry;
		friend class GameObject;
	};

} // namespace Mapo
