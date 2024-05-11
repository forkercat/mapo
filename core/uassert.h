//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include "core/logging.h"

#include <cassert>
#include <csignal>

#define MP_ASSERT(exp, ...)                              \
	do                                                   \
	{                                                    \
		if (!(exp))                                      \
		{                                                \
			MP_ERROR("ASSERT FAILED: {0}", __VA_ARGS__); \
			raise(SIGTRAP);                              \
		}                                                \
	}                                                    \
	while (0)

#define MP_ASSERT_EQ(x, y, ...) MP_ASSERT(x == y, __VA_ARGS__)
#define MP_ASSERT_NEQ(x, y, ...) MP_ASSERT(x != y, __VA_ARGS__)
