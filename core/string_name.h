//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#pragma once

#include "core/typedefs.h"
#include "core/string.h"

namespace mapo
{
	class SName final
	{
	public:
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

} // namespace mapo

namespace std
{
	template <>
	struct hash<mapo::SName>
	{
		size_t operator()(const mapo::SName& name) const
		{
			// Let's just return the hash value stored in it.
			return name.GetHash();
		}
	};

} // namespace std
