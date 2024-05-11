//
// Created by Junhao Wang (@forkercat) on 5/11/24.
//

#pragma once

#include "core/logging.h"

#define VK_CHECK(x)                                 \
	do                                              \
	{                                               \
		VkResult err = x;                           \
		if (err)                                    \
		{                                           \
			MP_ERROR("VULKAN ERROR: {}", (int)err); \
			abort();                                \
		}                                           \
	}                                               \
	while (0)
