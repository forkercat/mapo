//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#pragma once

#include "core/core.h"

#include <entt/entity/registry.hpp>

namespace mapo
{
	class Scene
	{
	public:
		Scene()
		{
		}

		void CreateGameObject()
		{
		}

		void DestroyGameObject()
		{
		}

	private:
		entt::registry m_registry;
	};

} // namespace mapo
