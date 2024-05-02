//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#pragma once

#include "core/typedefs.h"

#include <string> // included to support string operations for now.

namespace Mapo
{
	// TODO: Implement you own string class!
	using String = std::string;

	namespace StringOp
	{
		String Format(const char* fmt, ...);

	} // namespace StringOp

} // namespace Mapo
