//
// Created by Junhao Wang (@forkercat) on 4/28/24.
//

#include "string_name.h"

#include "core/uassert.h"
#include "core/templates/hash_map.h"

#include <string>

namespace Mapo
{
	/////////////////////////////////////////////////////////////////////////////////
	// Crc32
	/////////////////////////////////////////////////////////////////////////////////

	class Crc32
	{
	public:
		Crc32()
		{
			U32 polynomial = 0xEDB88320;
			for (U32 i = 0; i < 256; i++)
			{
				U32 c = i;
				for (size_t j = 0; j < 8; j++)
				{
					if (c & 1)
					{
						c = polynomial ^ (c >> 1);
					}
					else
					{
						c >>= 1;
					}
				}
				m_table[i] = c;
			}
		}

		U32 Compute(const char* data, size_t length)
		{
			MP_ASSERT(data);
			U32 crc = 0xFFFFFFFF;
			for (size_t i = 0; i < length; i++)
			{
				U8 index = (crc ^ data[i]) & 0xFF;
				crc = (crc >> 8) ^ m_table[index];
			}
			return crc ^ 0xFFFFFFFF;
		}

	private:
		U32 m_table[256];
	};

	static Crc32 s_crc32;

	MP_FORCE_INLINE U32 HashCrc32(const char* str)
	{
		return s_crc32.Compute(str, strlen(str));
	}

	/////////////////////////////////////////////////////////////////////////////////
	// SName
	/////////////////////////////////////////////////////////////////////////////////

	SName::SName(const String& str)
		: SName(str.c_str())
	{
	}

	static HashMap<U32, const char*> s_stringNameTable;

	U32 SName::InternString(const char* str)
	{
		U32 hashValue = HashCrc32(str);

		HashMap<U32, const char*>::iterator it = s_stringNameTable.find(hashValue);

		if (it == s_stringNameTable.end())
		{
			// This string name has not been added to table.
			// Make sure to copy it in case the original was dynamically allocated.
			s_stringNameTable[hashValue] = strdup(str);
		}

		return hashValue;
	}

	const char* SName::GetCString() const
	{
		HashMap<U32, const char*>::iterator it = s_stringNameTable.find(m_hashValue);

		MP_ASSERT(it != s_stringNameTable.end(), "Something terrible has happened :(");

		return it->second;
	}

} // namespace Mapo
