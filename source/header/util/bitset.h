#pragma once

namespace util 
{
	template<size_t bytes>
	class bitset
	{
		char m_bits[bytes];

	public:
		bitset() : m_bits{ 0 } { }

		constexpr bool at(size_t index) const 
		{
			return m_bits[index / 8] & (1 << (index % 8));
		}

		constexpr void set(size_t index, bool value)
		{
			//static_assert(index < bytes * 8, "Set index out of range");

			if (value)
			{
				m_bits[index / 8] |= (1ll << (index % 8));
			}
			else
			{
				m_bits[index / 8] &= ~(1ll << (index % 8));
			}
		}
	};
};