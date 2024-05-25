//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#include "scene.h"

#include "engine/scene/game_object.h"
#include "engine/scene/component.h"

namespace Mapo
{
	Scene::Scene()
	{
	}

	void Scene::OnUpdateRuntime(Timestep dt)
	{
		// TODO: For now let's do everything in editor.
	}

	void Scene::OnUpdateEditor(Timestep dt, EditorCamera& camera)
	{
		// Run scripts.
		m_registry.view<NativeScriptComponent>().each([=](entt::entity entityHandle, NativeScriptComponent& scriptComponent) {
			if (scriptComponent.runInEditor)
			{
				if (!scriptComponent.scriptable)
				{
					scriptComponent.scriptable = scriptComponent.InstantiateScript();
					scriptComponent.scriptable->m_gameObject = GameObject{ entityHandle, this };
					scriptComponent.scriptable->OnCreate();
				}

				scriptComponent.scriptable->OnUpdateEditor(dt);
			}
		});

		// Begin render.

		// End render.
	}

	GameObject Scene::CreateGameObject(const String& name)
	{
		GameObject gameObject = GameObject(m_registry.create(), this);

		gameObject.AddComponent<InfoComponent>().objectName = name.empty() ? "GameObject" : name;
		gameObject.AddComponent<TransformComponent>();

		return gameObject;
	}

	void Scene::DestroyGameObject(GameObject& gameObject)
	{
		m_registry.destroy(gameObject.m_entityHandle);
	}

	std::vector<GameObject> Scene::GetGameObjects()
	{
		std::vector<GameObject> gameObjects;

		auto gameObjectView = m_registry.view<TransformComponent>(); // all components

		// for (entt::entity entity : gameObjectView)
		for (auto iter = gameObjectView.rbegin(); iter != gameObjectView.rend(); ++iter)
		{
			// Use reserve iterator such that game objects are sorted on entity ids.
			gameObjects.push_back({ *iter, this });
		}

		return gameObjects;
	}

	template <typename T>
	void Scene::OnComponentAdded(GameObject& gameObject, T& component)
	{
	}

	template <>
	void Scene::OnComponentAdded<InfoComponent>(GameObject& gameObject, InfoComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded<TransformComponent>(GameObject& gameObject, TransformComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded<CameraComponent>(GameObject& gameObject, CameraComponent& component)
	{
		// Update viewport?
	}

	template <>
	void Scene::OnComponentAdded<MeshComponent>(GameObject& gameObject, MeshComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded<MaterialComponent>(GameObject& gameObject, MaterialComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded<NativeScriptComponent>(GameObject& gameObject, NativeScriptComponent& component)
	{
	}

} // namespace Mapo
