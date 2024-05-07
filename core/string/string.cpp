//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#include "string.h"

#include <cstdio>
#include <cstdarg>

#include "core/uassert.h"

#define MAX_FORMAT_STRING_LENGTH 256

namespace Mapo
{
	namespace StringOp
	{
		String Format(const char* fmt, ...)
		{
			char buffer[MAX_FORMAT_STRING_LENGTH];

			va_list args;
			va_start(args, fmt);

			int result = snprintf(buffer, MAX_FORMAT_STRING_LENGTH, fmt, args);
			MP_ASSERT(result >= 0 && result < MAX_FORMAT_STRING_LENGTH);

			va_end(args);

			return buffer;
		}

	} // namespace StringOp
} // namespace Mapo
