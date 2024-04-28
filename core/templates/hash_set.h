//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#pragma once

#include "core/typedefs.h"

#include <unordered_set>

namespace mapo
{
	// TODO: Implement your hash set class!
	template <typename T>
	using HashSet = std::unordered_set<T>;

} // namespace mapo
