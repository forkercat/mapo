//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#pragma once

#include "core/core.h"

#include "engine/scene/editor_camera.h"
#include "engine/scene/game_object.h"

#include <vulkan/vulkan.h>

namespace Mapo
{
	struct FrameInfo
	{
		U32 frameIndex;
		F32 frameTime;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet globalDescriptorSet;
		EditorCamera& camera;
		std::vector<GameObject>& gameObjects;
	};

} // namespace Mapo
