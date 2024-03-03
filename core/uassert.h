//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include <cassert>
#include <csignal>

#include "logging.h"

// #define ASSERT(x) assert(x)

#define ASSERT(x, ...)        \
	{                         \
		if (!(x))             \
		{                     \
			LOG(__VA_ARGS__); \
			raise(SIGTRAP);   \
		}                     \
	}
