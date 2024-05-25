//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "scene.h"

#include <entt/entity/registry.hpp>

namespace Mapo
{
	class GameObject final
	{
	public:
		virtual ~GameObject() = default;

		// Default constructor needed for temp object.
		GameObject() = default;
		GameObject(const GameObject& other) = default;

		operator bool() const { return m_entityHandle != entt::null; }
		MP_FORCE_INLINE bool IsValid() const { return m_entityHandle != entt::null; }

		bool operator==(const GameObject& other) const { return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene; }
		bool operator!=(const GameObject& other) const { return !(*this == other); }

		operator entt::entity() const { return m_entityHandle; }
		operator U32() const { return (U32)m_entityHandle; }

		/////////////////////////////////////////////////////////////////////////////////

		const String& GetName() const; // tag
		String&		  GetName();

		bool  IsEnabled() const { return m_enabled; }
		void  SetEnabled(bool enabled) { m_enabled = enabled; }
		bool& GetEnabled() { return m_enabled; }

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

			T& component = m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
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

		// Data
		bool m_enabled{ true };

		friend class Scene;
	};

} // namespace Mapo
