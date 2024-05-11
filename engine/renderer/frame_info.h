//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#pragma once

#include "core/core.h"

#include "engine/camera.h"
#include "engine/game_object.h"

#include <vulkan/vulkan.h>

namespace Mapo
{
	struct FrameInfo
	{
		U32 frameIndex;
		F32 frameTime;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet globalDescriptorSet;
		Camera& camera;
		std::vector<GameObject>& gameObjects;
	};

} // namespace Mapo
