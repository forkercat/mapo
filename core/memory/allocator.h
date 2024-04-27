//
// Created by Junhao Wang (@forkercat) on 4/26/24.
//

#pragma once

#include "core/typedefs.h"

#include <memory>

namespace mapo
{
	class IAllocator
	{
	public:
		virtual ~IAllocator()
		{
		}

		virtual void* Allocate(USize) = 0;
		virtual void Free(void*) = 0;

		virtual void FreeAll()
		{
		}
	};

	class StdAllocator : public IAllocator
	{
	public:
		void* Allocate(USize numBytes) override
		{
			// TODO: Consider alignment!
			return std::malloc(numBytes);
		}

		void Free(void* ptr) override
		{
			return std::free(ptr);
		}
	};

	// TODO: Implement this!
	// TODO: Add CustomAllocator
	class LinearAllocator : public IAllocator
	{
	public:
		explicit LinearAllocator(USize numBytes, void* start)
		{
		}

		void* Allocate(USize numBytes) override
		{
			return nullptr;
		}

		void Free(void* ptr) override
		{
			MP_ERROR("You should not be calling Free of a Linear allocator!");
			// BUG: MP_DELETE may call this Free().
		}

		void FreeAll() override
		{
		}
	};

} // namespace mapo
