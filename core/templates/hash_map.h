//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#pragma once

#include "core/typedefs.h"

#include <unordered_map>

namespace mapo
{
	// TODO: Implement your hash table class!
	template <typename K, typename V>
	using HashMap = std::unordered_map<K, V>;

} // namespace mapo
