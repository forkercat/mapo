//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#pragma once

#include "core/core.h"

#include "camera.h"
#include "game_object.h"

#include <vulkan/vulkan.h>

namespace mapo
{
	struct VulkanFrameInfo
	{
		U32 frameIndex;
		F32 frameTime;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet globalDescriptorSet;
		Camera& camera;
		std::vector<GameObject>& gameObjects;
	};

} // namespace mapo
