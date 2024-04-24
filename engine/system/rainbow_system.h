//
// Created by Junhao Wang (@forkercat) on 4/18/24.
//

#pragma once

#include "core/core.h"

#include "engine/game_object.h"

#include <memory>
#include <random>
#include <vector>

namespace mapo
{
	class RainbowSystem
	{
	public:
		RainbowSystem(F32 flickerRate)
			: m_flickerRate(flickerRate)
		{
			// Initialize colors.
			m_colors = { { 0.8f, 0.1f, 0.1f }, { 0.1f, 0.8f, 0.1f }, { 0.1f, 0.1f, 0.8f },
				{ 0.8f, 0.8f, 0.1f }, { 0.8f, 0.1f, 0.8f }, { 0.1f, 0.8f, 0.8f } };

			m_elapsedTime = m_flickerRate;
		}

		// Randomly select a color for each game object every m_flickerRate seconds.
		void Update(std::vector<GameObject>& gameObjects)
		{
			F32 deltaTime = 0;

			m_elapsedTime -= deltaTime;

			if (m_elapsedTime < 0.0f)
			{
				m_elapsedTime += m_flickerRate;

				std::uniform_int_distribution<int> randInt{ 0, static_cast<int>(m_colors.size()) - 1 };

				for (auto& object : gameObjects)
				{
					int randValue = randInt(m_rng);
					object.color = m_colors[randValue];
				}
			}
		}

	private:
		std::random_device m_rd;
		std::mt19937 m_rng{ m_rd() };

		std::vector<glm::vec3> m_colors;

		F32 m_flickerRate;
		F32 m_elapsedTime;
	};

} // namespace mapo
