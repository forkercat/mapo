//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "model.h"

#include <memory>
#include <unordered_map>

namespace mapo
{
	/////////////////////////////////////////////////////////////////////////////////
	// Components
	/////////////////////////////////////////////////////////////////////////////////

	struct TransformComponent
	{
		Vector3 translation{};
		Vector3 rotation{};
		Vector3 scale{ 1.0f, 1.0f, 1.0f };

		Matrix4 GetTransform();

		Matrix3 GetNormalMatrix();
	};

	/////////////////////////////////////////////////////////////////////////////////
	// GameObject
	/////////////////////////////////////////////////////////////////////////////////

	class GameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GameObject>;

	public:
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		static GameObject CreateGameObject()
		{
			static id_t currentId = 0;
			return GameObject(currentId++);
		}

		id_t GetId() { return m_id; }

	public:
		Ref<Model> model{};
		Vector3 color{};

		// Components
		TransformComponent transform;

	private:
		GameObject(id_t objectId)
			: m_id(objectId)
		{
		}

	private:
		id_t m_id;
	};

} // namespace mapo
