//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#pragma once

#include "core/core.h"

#include <entt/entity/registry.hpp>

#include <vector>

namespace mapo
{
	class GameObject;

	class Scene
	{
	public:
		Scene();

		void OnUpdate(F32 dt);

		GameObject CreateGameObject(const String& name = "");

		void DestroyGameObject(const GameObject& gameObject);

		// TODO: Should not do this.
		std::vector<GameObject> GetGameObjects();

	private:
		entt::registry m_registry;
		friend class GameObject;
	};

} // namespace mapo
