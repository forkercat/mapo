//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include <cassert>
#include <csignal>

#include "core/logging.h"

#define ASSERT(exp, ...)        \
	do                          \
	{                           \
		if (!(exp))             \
		{                       \
			PRINT(__VA_ARGS__); \
			raise(SIGTRAP);     \
		}                       \
	} while (0)

#define ASSERT_EQ(x, y, ...) ASSERT(x == y, __VA_ARGS__)
#define ASSERT_NEQ(x, y, ...) ASSERT(x != y, __VA_ARGS__)
