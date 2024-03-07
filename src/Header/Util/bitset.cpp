#include "../commons.h"
#include "bitset.h"

template<size_t bytes>
constexpr bool util::bitset<bytes>::at(size_t index) const
{
	return m_bits[index / 8] & (1 << (index % 8));
}

template<size_t bytes>
constexpr void util::bitset<bytes>::set(size_t index, bool value)
{
	static_assert(index < bytes * 8, "Set index out of range");

	if (value)
	{
		m_bits[index / 8] |= (1 << (index % 8));
	}
	else
	{
		m_bits[index / 8] &= ~(1 << (index % 8));
	}
}