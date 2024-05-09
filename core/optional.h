//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

namespace Mapo
{
	template <typename Type>
	class Optional
	{
	public:
		virtual ~Optional() = default;

		Optional() = default;

		Optional(const Optional& value)
		{
			m_hasValue = value.m_hasValue;
			m_value = value.m_value;
		}

		template <typename U = Type>
		Optional(const U& value)
		{
			m_hasValue = true;
			m_value = value;
		}

		bool HasValue() const
		{
			return m_hasValue;
		}

		const Type& value() const
		{
			MP_ASSERT(m_hasValue, "Value does not exist!");
			return m_value;
		}

		const Type ValueOr(Type&& alternative) const
		{
			if (HasValue())
			{
				return m_value;
			}

			return alternative;
		}

		const Type ValueOr(const Type& alternative = {}) const
		{
			if (HasValue())
			{
				return m_value;
			}

			return alternative;
		}

		Optional& operator=(Optional&& other)
		{
			m_hasValue = other.m_hasValue;
			m_value = other.m_value;
			return *this;
		}

		template <typename U = Type>
		Optional& operator=(U&& value)
		{
			m_hasValue = true;
			m_value = value;
			return *this;
		}

		Optional& operator=(const Optional& other)
		{
			m_hasValue = other.m_hasValue;
			m_value = other.value;
			return *this;
		}

		template <typename U = Type>
		Optional& operator=(U* value)
		{
			if (value == nullptr)
			{
				m_hasValue = false;
				return *this;
			}

			m_hasValue = true;
			m_value = *value;
			return *this;
		}

	private:
		bool m_hasValue = false;
		Type m_value;
	};

} // namespace Mapo
