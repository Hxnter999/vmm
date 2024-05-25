#pragma once
#undef min
#undef max

namespace util {
	template <typename T>
	inline const T& min(const T& a, const T& b) {
		return (b < a) ? b : a;
	}

	template <typename T>
	inline const T& max(const T& a, const T& b) {
		return (a < b) ? b : a;
	}
}