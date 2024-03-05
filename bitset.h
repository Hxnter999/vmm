#pragma once
namespace Util 
{
	template<size_t bytes>
	class bitset
	{
		char[bytes] m_bits;

	public:
		bitset() : m_bits{} { }

		constexpr bool at(size_t index) const;

		constexpr void set(size_t index, bool value);
	};

};