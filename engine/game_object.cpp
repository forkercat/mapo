//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "game_object.h"

#include "engine/component.h"

namespace Mapo
{
	GameObject::GameObject(entt::entity entity, Scene* scene)
		: m_entityHandle(entity), m_scene(scene)
	{
	}

	const String& GameObject::GetName() const
	{
		return m_scene->m_registry.get<TagComponent>(m_entityHandle).tag;
	}

	void GameObject::OnUpdate(const F32 dt)
	{
	}

} // namespace Mapo
