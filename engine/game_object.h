//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/scene.h"

#include <entt/entity/registry.hpp>

namespace Mapo
{
	class GameObject final
	{
	public:
		virtual ~GameObject() = default;

		operator bool() const { return m_entityHandle != entt::null; }
		MP_FORCE_INLINE bool IsValid() const { return m_entityHandle != entt::null; }

		const String& GetName() const;

		void OnUpdate(const F32 dt);

		/////////////////////////////////////////////////////////////////////////////////
		// Component functions
		/////////////////////////////////////////////////////////////////////////////////

		template <typename T>
		bool HasComponent()
		{
			return m_scene->m_registry.all_of<T>(m_entityHandle);
		}

		template <typename T>
		T& GetComponent()
		{
			MP_ASSERT(HasComponent<T>(), "The entity does not have this type of components!");
			return m_scene->m_registry.get<T>(m_entityHandle);
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			MP_ASSERT(!HasComponent<T>(), "The entity already has this type of components!");
			return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		void RemoveComponent()
		{
			MP_ASSERT(HasComponent<T>(), "The entity does not have this type of components!");
			m_scene->m_registry.remove<T>(m_entityHandle);
		}

	private:
		// Make this private since game objects are created through a scene object.
		GameObject(entt::entity entity, Scene* scene);

	private:
		// communicate with the EnTT registry.
		entt::entity m_entityHandle{ entt::null };
		Scene*		 m_scene{ nullptr };

		friend class Scene;
	};

} // namespace Mapo
