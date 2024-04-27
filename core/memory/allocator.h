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
	};

	class StdAllocator : public IAllocator
	{
	public:
		void* Allocate(USize size) override
		{
			// TODO: Consider alignment!
			return std::malloc(size);
		}

		void Free(void* ptr) override
		{
			std::free(ptr);
		}
	};

	class LinearAllocator : public IAllocator
	{
	public:
		explicit LinearAllocator(USize size)
			: m_base(nullptr), m_current(nullptr), m_totalSize(size)
		{
			m_base = new U8[size];
			m_secondBase = m_base;
			m_current = m_base;
		}

		~LinearAllocator()
		{
			ASSERT_EQ(m_base, m_secondBase, "The base of the linear allocator has been altered!");
			delete[] m_base;
		}

		void* Allocate(USize size) override
		{
			USize newSize = m_current + size - m_base;

			if (newSize <= m_totalSize)
			{
				void* result = m_current;
				m_current += size;
				return result;
			}
			else
			{
				MP_ERROR("Failed to allocate %zu bytes of memory!", size);
				return nullptr;
			}
		}

		void Free(void* ptr) override
		{
			// MP_ERROR("You should not be calling Free of a Linear allocator!");
		}

		void Reset()
		{
			m_current = m_base;
		}

	private:
		U8* m_base = nullptr;
		U8* m_secondBase = nullptr;

		U8* m_current = nullptr;
		const USize m_totalSize{};
	};

} // namespace mapo
