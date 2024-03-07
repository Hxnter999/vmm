#pragma once

namespace util 
{
	template<size_t bytes>
	class bitset
	{
		char m_bits[bytes];

	public:
		bitset() : m_bits{} { }

		constexpr bool at(size_t index) const;

		constexpr void set(size_t index, bool value);
	};
};