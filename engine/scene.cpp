//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#include "scene.h"

#include "engine/game_object.h"
#include "engine/component.h"

namespace Mapo
{
	Scene::Scene()
	{
	}

	void Scene::OnUpdate(F32 dt)
	{
	}

	GameObject Scene::CreateGameObject(const String& name)
	{
		GameObject gameObject = GameObject(m_registry.create(), this);

		gameObject.AddComponent<TransformComponent>();
		gameObject.AddComponent<TagComponent>().tag = name.empty() ? "GameObject" : name;

		return gameObject;
	}

	void Scene::DestroyGameObject(const GameObject& gameObject)
	{
		m_registry.destroy(gameObject.m_entityHandle);
	}

	std::vector<GameObject> Scene::GetGameObjects()
	{
		std::vector<GameObject> gameObjects;

		auto gameObjectView = m_registry.view<TransformComponent>(); // all components

		for (entt::entity entity : gameObjectView)
		{
			gameObjects.push_back({ entity, this });
		}

		return gameObjects;
	}

} // namespace Mapo
