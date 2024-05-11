//
// Created by Junhao Wang (@forkercat) on 4/26/24.
//

#pragma once

#include "core/typedefs.h"
#include "core/uassert.h"

#include <memory>

namespace Mapo
{
	/////////////////////////////////////////////////////////////////////////////////
	// Method 1: Using bit-shift operation to align.
	//
	// Alignment works by stripping off least-significant bits defined by alignment (i.e. mask).
	// Given an address and a desired alignment L, we can align that address to an L-byte
	// boundary by first adding (L - 1) ot it, and then stripping off (by & ~mask) the
	// N least-significant bits of the resulting address, where N = log2(L).
	// The reason why we first need to add (L - 1) is we want the address to be over the next
	// alignment boundary before stripping/masking off lower bits.
	// For instance, L=8 (0000 1000) and the next alignment boundary is L=16 (0001 0000).
	/////////////////////////////////////////////////////////////////////////////////

	// Shift the given address upwards if necessary to ensure it is aligned
	// to the given number of bytes.
	inline uintptr_t AlignAddress(uintptr_t address, U8 alignment)
	{
		// Example:
		//   alignment = 4 (0000 0100) | mask = 3 (0000 0011)
		//   alignment = 8 (0000 1000) | mask = 7 (0000 0111)
		const size_t mask = alignment - 1;
		MP_ASSERT((alignment & mask) == 0, "Alignment is not power of two!");

		// Example:
		//   address   = 14  (0000 1110)   aligned = 16 (0001 0000)
		//   alignment = 8   (0000 1000)
		//   mask      = 7   (0000 0111)
		//   ~mask           (1111 1000)
		return (address + mask) & ~mask;
	}

	template <typename T>
	inline T* AlignPointer(T* ptr, U8 alignment)
	{
		// Cast to unsigned long for pointer arithmetic operations.
		const uintptr_t original = reinterpret_cast<uintptr_t>(ptr);
		const uintptr_t aligned = AlignAddress(original, alignment);
		return reinterpret_cast<T*>(aligned);
	}

	// Return an aligned memory block using malloc. We simply allocate a little bit more
	// memory than was actually requested, which enables us to shift the address of the
	// memory block upward slightly so that it is properly aligned, then return a shifted address.
	// NOTE: This implementation doesn't take care of that.
	//
	// Example: numBytes = 3 | alignment = 8
	//   The returned ptr could be between [0x00, 0x07] and the worst case would be at 0x1.
	//   And it needs extra 7 bytes to hit 0x8. The worstCaseBytes is 10 (= 3 + 8 - 1).
	//   - For ptr=0x0, the block is [0x00, 0x0A] = (<3> + 7). The aligned ptr is 0x00.
	//   - For ptr=0x1, the block is [0x01, 0x0B] = (7 + <3>). The aligned ptr is 0x08.
	//   - For ptr=0x7, the block is [0x07, 0x11] = (1 + <3> + 6). The aligned ptr is 0x08.
	//
	// TODO: Make it to source files.
	inline void* AllocateAligned_VersionOne(size_t numBytes, U8 alignment = 4)
	{
		// Determine the actual number of bytes we need. (alignment - 1) defines the space we can shift.
		size_t actualBytes = numBytes + alignment - 1;
		// Allocate unaligned block.
		U8* pOriginal = new U8[actualBytes]; // new may throw if out of memory
		// Align the block. The block is now large enough such that we can align the ptr.
		return AlignPointer(pOriginal, alignment);
	}

	// In version one, we don't take care of storing the shift which is needed to properly call Free().
	// We can either store the original address (8 bytes) or store the shift in just one byte.
	// In the above implementation, we allocate extra (alignment - 1) bytes to give us room to
	// align the pointer. It is also a place to store the shift value. However, if the original
	// address returned by new is already aligned, the above code won't shift the original at all,
	// meaning that there won't be any room for us to store the shift value.
	//
	// To address this, we can allocate (alignment) extra bytes instead of (alignment - 1), then
	// we always shift the original pointer up to the next alignment boundary, even already aligned.
	// Now the maximum shift is (alignment) bytes, and the minimum shift is 1 byte, which means
	// there is always one byte we can use for the shift value.
	//
	// Storing the shift in one byte works for alignments up to and including 128 bytes.
	// Since we won't shift zero bytes, we can make (alignment = 0) to shift 256 bytes.
	//
	inline void* AllocateAligned(size_t numBytes, U8 alignment = 4)
	{
		size_t actualBytes = numBytes + alignment;
		U8* pOriginal = new U8[actualBytes];
		// Align the block. If no alignment occurred, shift it up to next alignment boundary.
		U8* pAligned = AlignPointer(pOriginal, alignment);
		if (pOriginal == pAligned)
		{
			pAligned += alignment;
		}
		// Determine the shift and store it.
		ptrdiff_t shift = pAligned - pOriginal;
		MP_ASSERT(shift > 0 && shift <= 256, "Shift is too large!");
		pAligned[-1] = static_cast<U8>(shift);
		return pAligned;
	}

	inline void FreeAligned(void* ptr)
	{
		if (ptr)
		{
			U8* pAligned = static_cast<U8*>(ptr);
			// Extract the shift and the original ptr.
			ptrdiff_t shift = pAligned[-1];
			if (shift == 0)
				shift = 256;
			U8* pOriginal = pAligned - shift;
			delete[] pOriginal;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Method 2: Using modulo operator to compute the offset to move the malloced address
	// to get the required alignment. The offset can be stored using just 16-bit such that
	// we can properly call free().
	/////////////////////////////////////////////////////////////////////////////////

	inline void* AllocateAligned_Modulo(size_t numBytes, U8 alignment = 4)
	{
		size_t actualBytes = numBytes + alignment;
		U8* pOriginal = new U8[actualBytes];
		// Calculate the shift.
		uintptr_t original = reinterpret_cast<uintptr_t>(pOriginal);
		size_t shift = alignment - original % alignment;
		MP_ASSERT(shift > 0 && shift <= 256, "Shift is too large!");
		// Align and store the shift.
		uintptr_t aligned = original + shift;
		U8* pAligned = reinterpret_cast<U8*>(aligned);
		pAligned[-1] = static_cast<U8>(shift);
		return pAligned;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Allocators
	/////////////////////////////////////////////////////////////////////////////////

	// Allocator interface that customer allocator implements.
	class IAllocator
	{
	public:
		virtual ~IAllocator()
		{
		}

		virtual void* Allocate(size_t) = 0;
		virtual void Free(void*) = 0;
	};

	class StdAllocator : public IAllocator
	{
	public:
		void* Allocate(size_t size) override
		{
			// TODO: Consider alignment!
			return std::malloc(size);
		}

		void Free(void* ptr) override
		{
			std::free(ptr);
		}

		static IAllocator& Get()
		{
			static StdAllocator allocator;
			return allocator;
		}
	};

	class LinearAllocator : public IAllocator
	{
	public:
		explicit LinearAllocator(size_t size)
			: m_base(nullptr), m_current(nullptr), m_totalSize(size)
		{
			m_base = new U8[size];
			m_secondBase = m_base;
			m_current = m_base;
		}

		~LinearAllocator()
		{
			MP_ASSERT_EQ(m_base, m_secondBase, "The base of the linear allocator has been altered!");
			delete[] m_base;
		}

		void* Allocate(size_t size) override
		{
			size_t newSize = m_current + size - m_base;

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
		const size_t m_totalSize{};
	};

} // namespace Mapo
