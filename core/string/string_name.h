//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#pragma once

#include "core/typedefs.h"
#include "string.h"

namespace Mapo
{
	class SName final
	{
	public:
		virtual ~SName() = default;

		explicit SName(const char* str)
		{
			m_hashValue = InternString(str);
		}

		SName(const String& str);

		SName()
			: SName("")
		{
		}

		MP_FORCE_INLINE static SName Empty()
		{
			return {};
		}

		U32 GetHash() const { return m_hashValue; }

		MP_FORCE_INLINE bool operator==(const SName& name) const
		{
			return m_hashValue == name.m_hashValue;
		}

		const char* GetCString() const;

	private:
		// Intern a c-style string and return a SName.
		// If the string was first seen, it would be added to an internal hash map.
		U32 InternString(const char* str);

	private:
		U32 m_hashValue{};
	};

} // namespace Mapo

namespace std
{
	template <>
	struct hash<Mapo::SName>
	{
		size_t operator()(const Mapo::SName& name) const
		{
			// Let's just return the hash value stored in it.
			return name.GetHash();
		}
	};

} // namespace std
