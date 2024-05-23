#pragma once
#include <commons.h>

namespace util
{
	template <typename T>
	inline T* allocate_pool(POOL_TYPE type = NonPagedPoolNx, size_t size = sizeof(T)) {
		auto result = static_cast<T*>(ExAllocatePoolWithTag(type, size, ' mmv')); 
		if (result) {
			memset(result, 0, size);
		}
		return result;
	}

	inline void free_pool(void* ptr) {
		if (ptr) {
			ExFreePoolWithTag(ptr, ' mmv');
		}
	}
}