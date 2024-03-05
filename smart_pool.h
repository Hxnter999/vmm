#pragma
#include "commons.h"

template<typename T, size_t Tag = 0 >
class SmartPool
{
	T* pool;
	size_t size;

public:

	explicit SmartPool(T* pool) : pool(pool), size(sizeof(T)) {}

	template<POOL_TYPE pooltype = NonPagedPool>
	explicit SmartPool() : size(sizeof(T)) 
	{
		pool = static_cast<T*>(ExAllocatePoolWithTag(pooltype, size, Tag));
	}

	template<size_t size, POOL_TYPE pooltype = NonPagedPool>
	explicit SmartPool() : size(size)
	{
		pool = static_cast<T*>(ExAllocatePoolWithTag(pooltype, size, Tag));
	}
	
	T* get() const { return pool; }

	operator bool() const { return pool != nullptr; }

	T* operator->() const { return pool; }

	T& operator*() const { return *pool; }

	T operator[](size_t index) const { return pool[index]; }

	T at(size_t index) const 
	{
		if(index < size)
			return pool[index];

		return T{};
	}

	SmartPool(const SmartPool&) = delete;
	SmartPool& operator=(const SmartPool&) = delete;

	SmartPool(SmartPool&& other) noexcept : pool(other.pool) { other.pool = nullptr; }
	SmartPool& operator=(SmartPool&& other) noexcept
	{
		if (this != &other)
		{
			pool = other.pool;
			other.pool = nullptr;
		}
		return *this;
	}

	~SmartPool() { if (pool) ExFreePoolWithTag(pool, Tag); }
};
