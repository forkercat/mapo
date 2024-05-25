//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "game_object.h"

#include "engine/scene/component.h"

namespace Mapo
{
	GameObject::GameObject(entt::entity entity, Scene* scene)
		: m_entityHandle(entity), m_scene(scene)
	{
	}

	const String& GameObject::GetName() const
	{
		return m_scene->m_registry.get<InfoComponent>(m_entityHandle).objectName;
	}

	String& GameObject::GetName()
	{
		return m_scene->m_registry.get<InfoComponent>(m_entityHandle).objectName;
	}

	void GameObject::OnUpdate(const F32 dt)
	{
	}

} // namespace Mapo
