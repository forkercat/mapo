//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include <cstdint>
#include <memory>

namespace mapo
{
	// Integers
	using U64 = std::uint64_t;
	using U32 = std::uint32_t;
	using U16 = std::uint16_t;
	using U8 = std::uint8_t;

	using I64 = std::int64_t;
	using I32 = std::int32_t;
	using I16 = std::int16_t;
	using I8 = std::int8_t;

	using USize = size_t;

// Floats
#ifdef FORCE_FLOAT_64
	using F32 = double;
#else
	using F32 = float;
#endif
	using F64 = double;

	// Reference
	template <typename T>
	using UniqueRef = std::unique_ptr<T>;

	template <typename T, typename... Args>
	constexpr UniqueRef<T> MakeUniqueRef(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

} // namespace mapo

#define BIT(x) (1 << x)
#define STR(x) #x

// Inline macros
#ifndef MP_ALWAYS_INLINE
	#if defined(__GNUC__)
		#define MP_ALWAYS_INLINE __attribute__((always_inline)) inline
	#elif defined(_MSC_VER)
		#define MP_ALWAYS_INLINE __forceinline
	#else
		#define MP_ALWAYS_INLINE inline
	#endif
#endif

#ifndef MP_FORCE_INLINE
	#ifdef NDEBUG
		#define MP_FORCE_INLINE MP_ALWAYS_INLINE
	#else
		#define MP_FORCE_INLINE inline
	#endif
#endif
