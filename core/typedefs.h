//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include <cstdint>
#include <memory>

namespace Mapo
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
	constexpr UniqueRef<T> MakeUnique(Args&&... args)
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

} // namespace Mapo

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

// No discard
#ifndef MP_NO_DISCARD
	#define MP_NO_DISCARD [[nodiscard]]
#endif

// Allow discard
#ifndef MP_ALLOW_DISCARD
	#define MP_ALLOW_DISCARD (void)
#endif

// ImGui
#define ICON_NAME(icon, name) icon "  " name
#define ICON_NAME2(icon, name) String(icon) + "  " + name

#define INVALID_GIZMO_TYPE -1
